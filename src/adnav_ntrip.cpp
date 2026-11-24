/****************************************************************/
/*                                                              */
/*                       Advanced Navigation                    */
/*               			NTRIP Client			  		    */
/*          Copyright 2023, Advanced Navigation Pty Ltd         */
/*                                                              */
/****************************************************************/
/*
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "adnav_ntrip.h"

namespace adnav::ntrip {

    bool Client::run(void) {
        if (service_is_running_.load()) return true;

        if (protocol_ != NTRIP_PROTOCOL_AUTO) {
            return run_with_protocol(protocol_);
        }

        // A number of older casters reject the HTTP/1.1 + Ntrip-Version header
        // instead of ignoring it.  Try v2 first, then retry the same endpoint
        // with the NTRIP v1 handshake only for negotiation-style failures.
        if (run_with_protocol(NTRIP_PROTOCOL_V2)) return true;

        const auto v2_failure = service_failure_.load();
        if (v2_failure == NTRIP_UNRECOGNIZED_RETURN ||
            v2_failure == NTRIP_REMOTE_CLOSE ||
            v2_failure == NTRIP_CONNECTION_TIMEOUT_FAILURE) {
            log_fn_("NTRIP v2 handshake failed; retrying with NTRIP v1.\r\n");
            return run_with_protocol(NTRIP_PROTOCOL_V1);
        }

        return false;
    }

    bool Client::run_with_protocol(adnav_ntrip_protocol_e protocol) {
        // Don't restart service if it is running.
        if(service_is_running_.load()) return true;

        service_failure_.store(NTRIP_NO_FAIL);

        // Closes any current connection and opens new connection
        if (!establish_new_connection()) return false;

        // check to see if the gga string needs regeneration.
        if (!gga_update_.load()) {
            adnav::utils::GenerateGGAString(gga_string_, latitude_, longitude_,
                altitude_, gnss_fix_, sats_, hdop_);
            gga_update_.store(true);
        }

        // Form the corrections request using the selected protocol's handshake.
        const std::string request = build_request(false, protocol);

        if (send(socket_fd_, request.c_str(), request.size(), 0) < 0) {
            log_err_fn_("Sending HTTP request failed!\r\n");
            service_failure_.store(NTRIP_SEND_REQUEST_FAILURE);
            stop();
            return false;
        }

        int ret = -1;
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(NTRIP_BUFFER_SIZE);

        int timeout = NTRIP_TIMEOUT_PERIOD * 10;
        while(timeout--) {
            // Check the socket for data
            ret = recv(socket_fd_, buffer.get(), NTRIP_BUFFER_SIZE, 0);

            // If data is present
            if (ret > 0) {
                // Reset the timeout.
                timeout = NTRIP_TIMEOUT_PERIOD * 10;

                // Store the buffered data into a string
                std::string result(buffer.get(), ret);

                // Check to see if there is a recognized header.  Match status
                // codes independently of HTTP minor version so v1 error
                // responses are not reported as an opaque "unknown" error.
                if (response_has_status(result, 200)) {

                    // Break the waiting loop.
                    break;

                } else if(response_has_status(result, 401))
                {
                    log_err_fn_("NTRIP Server Access Unauthorized.\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_UNAUTHORIZED);
                    stop();
                    return false;
                } else if(response_has_status(result, 403))
                {
                    log_err_fn_("NTRIP Server Access Forbidden.\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_FORBIDDEN);
                    stop();
                    return false;
                } else if(response_has_status(result, 404))
                {
                    log_err_fn_("NTRIP Server Resource Not Found.\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_NOT_FOUND);
                    stop();
                    return false;
                }
                else
                {
                    log_err_fn_("Unrecognized Return Header\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_UNRECOGNIZED_RETURN);
                    stop();
                    return false;
                }
            } else if (ret == 0) {
                log_err_fn_("Ntrip caster terminated connection.\r\n");
                service_failure_.store(NTRIP_REMOTE_CLOSE);
                stop();
                return false;
            }

            // Sleep for 100ms
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Test to see if the above loop broke due to timeout.
        if (timeout <= 0) {
            log_err_fn_("NtripCaster[" + server_ip_ + ":" + std::to_string(server_port_) +
                    " " + user_ + " " + passwd_ + " " + mountpoint_ + "] access timeout!\r\n");
            service_failure_.store(NTRIP_CONNECTION_TIMEOUT_FAILURE);
            stop();
            return false;
        }

        protocol_ = protocol;

        // Start the thread. join it if it is currently running.
        if (thread_.joinable()) thread_.join();
        thread_ = std::move(std::thread(&adnav::ntrip::Client::thread_handler, this));

        return true;
    }

    bool Client::retrieve_sourcetable(void) {
        if (protocol_ != NTRIP_PROTOCOL_AUTO) {
            return retrieve_sourcetable_with_protocol(protocol_);
        }

        if (retrieve_sourcetable_with_protocol(NTRIP_PROTOCOL_V2)) return true;

        const auto v2_failure = service_failure_.load();
        if (v2_failure == NTRIP_UNRECOGNIZED_RETURN ||
            v2_failure == NTRIP_REMOTE_CLOSE ||
            v2_failure == NTRIP_CONNECTION_TIMEOUT_FAILURE) {
            log_fn_("NTRIP v2 sourcetable request failed; retrying with NTRIP v1.\r\n");
            return retrieve_sourcetable_with_protocol(NTRIP_PROTOCOL_V1);
        }

        return false;
    }

    bool Client::retrieve_sourcetable_with_protocol(adnav_ntrip_protocol_e protocol) {
        service_failure_.store(NTRIP_NO_FAIL);

        // Closes any current on
        if (!establish_new_connection()) return false;
        int ret = -1;
        std::unique_ptr<char[]>  buffer = std::make_unique<char[]>(NTRIP_BUFFER_SIZE);

        // Form the Sourcetable request
        const std::string request = build_request(true, protocol);

        if (send(socket_fd_, request.c_str(), request.size(), 0) < 0) {
            log_err_fn_("Sending SOURCETABLE request failed!\r\n");
            service_failure_.store(NTRIP_SEND_REQUEST_FAILURE);
            stop();
            return false;
        }

        // Loop for NTRIP_TIMEOUT_PERIOD in seconds.
        int timeout = NTRIP_TIMEOUT_PERIOD * 10; // converts s to ms intervals
        bool header = false;
        while(timeout--) {
            // Check the socket for data
            ret = recv(socket_fd_, buffer.get(), NTRIP_BUFFER_SIZE, 0);

            // If data is present
            if (ret > 0) {
                // reset the timeout.
                timeout = NTRIP_TIMEOUT_PERIOD * 10;

                // store the buffered data into a string
                std::string result(buffer.get(), ret);

                // Check to see if there is a recognized header or seen a recognized header previously.
                if (response_has_status(result, 200)) {

                    // Print out the request result using the logging function.
                    log_fn_("Sourcetable response from " + server_ip_ + ":" +
                        std::to_string(server_port_) + "\r\n" + result);
                    header = true;

                }
                else if (header)
                {
                    log_fn_(result);
                }
                else if (response_has_status(result, 401))
                {
                    log_err_fn_("NTRIP Sourcetable access unauthorized.\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_UNAUTHORIZED);
                    stop();
                    return false;
                }
                else if (response_has_status(result, 403))
                {
                    log_err_fn_("NTRIP Sourcetable access forbidden.\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_FORBIDDEN);
                    stop();
                    return false;
                }
                else if (response_has_status(result, 404))
                {
                    log_err_fn_("NTRIP Sourcetable not found.\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_NOT_FOUND);
                    stop();
                    return false;
                }
                else
                {
                    log_err_fn_("Unrecognized Sourcetable Header\r\n" + result + "\r\n");
                    service_failure_.store(NTRIP_UNRECOGNIZED_RETURN);
                    stop();
                    return false;
                }
            } else if (ret == 0) {
                log_fn_("Ntrip caster ended connection.\r\n");
                stop();
                return true;
            }

            // sleep for 100ms
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        service_failure_.store(NTRIP_CONNECTION_TIMEOUT_FAILURE);
        stop();

        return false;
    }

    std::string Client::build_request(bool sourcetable, adnav_ntrip_protocol_e protocol) const {
        std::stringstream request;
        const std::string path = sourcetable ? "/" : "/" + mountpoint_;

        if (protocol == NTRIP_PROTOCOL_V1) {
            request << "GET " << path << " HTTP/1.0\r\n";
        } else {
            request << "GET " << path << " HTTP/1.1\r\n" <<
                "Host: " << server_ip_ << ":" << std::to_string(server_port_) << "\r\n" <<
                "Ntrip-Version: Ntrip/2.0\r\n";
        }

        request << "User-Agent: " << user_agent_ << "\r\n" <<
            "Accept: */*\r\n" <<
            "Authorization: Basic " << encode_credentials() << "\r\n";

        // Ntrip-GGA is a v2 request header.  The v1-compatible behavior is to
        // send the GGA sentence on the established stream, which thread_handler
        // already does periodically.
        if (!sourcetable && protocol == NTRIP_PROTOCOL_V2) {
            request << "Ntrip-GGA: " << gga_string_ << "\r\n";
        }

        request << "Connection: close\r\n\r\n";
        return request.str();
    }

    bool Client::response_has_status(const std::string& response, int status_code) const {
        const std::size_t line_end = response.find("\r\n");
        const std::string status_line = response.substr(0, line_end);
        return status_line.find(" " + std::to_string(status_code)) != std::string::npos;
    }

    void Client::stop(void) {
        // Store false in the atomic bool
        service_is_running_.store(false);

        // Close open sockets.
        #if defined(WIN32) || defined (_WIN32)
            if(socket_fd_ != INVALID_SOCKET) {
                closesocket(socket_fd_);
                WSACleanup();
                socket_fd_ = INVALID_SOCKET;
            }
        #else
            if (socket_fd_ > 0) {
                close(socket_fd_);
                socket_fd_ = -1;
            }
        #endif

        // Join the thread
        if(thread_.joinable()) thread_.join();
    }

    void Client::thread_handler(void) {
        service_is_running_.store(true);
        int ret;

        std::unique_ptr<char[]>buffer = std::make_unique<char[]>(NTRIP_BUFFER_SIZE);

        std::chrono::_V2::steady_clock::time_point gga_beg = std::chrono::steady_clock::now();
        std::chrono::_V2::steady_clock::time_point gga_end = gga_beg;
        const int gga_interval_ms = report_interval_ * 1000;

        std::chrono::_V2::steady_clock::time_point to_beg = gga_end;
        std::chrono::_V2::steady_clock::time_point to_end = to_beg;
        const int sock_timeout_ms = NTRIP_TIMEOUT_PERIOD * 1000;

        log_fn_("NtripClient service running...\r\n");

        // Loop continuously until told otherwise or an error occurs.
        while (service_is_running_.load()) {
            ret = recv(socket_fd_, buffer.get(), NTRIP_BUFFER_SIZE, 0);

            // If the socket has been closed
            if (ret == 0) {
                log_err_fn_("Remote has closed the socket!\r\n");
                service_failure_.store(NTRIP_REMOTE_CLOSE);
                break;
            } else if (ret < 0) { // socket has errored.
                const int receive_error = errno;
                // Check errno to see if it has errored due to an error
                // and not due to no data on non-blocking socket.
                if ((receive_error !=0) && (receive_error != EAGAIN) &&
                    (receive_error != EWOULDBLOCK) && (receive_error != EINTR)) {
                    log_err_fn_("Remote socket error, errno = " + std::to_string(receive_error) +
                        "\r\nstderr msg: " + std::strerror(receive_error) + "\r\n");
                    service_failure_.store(NTRIP_REMOTE_SOCKET_FAILURE);
                    break;
                }

                if (receive_error == EAGAIN || receive_error == EWOULDBLOCK) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                // no data recieved check for timeout.
                to_end = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    to_end - to_beg).count() >= sock_timeout_ms) {
                        service_failure_.store(NTRIP_CONNECTION_TIMEOUT_FAILURE);
                        break;
                }

            } else { // ret > 0 (data has been received)
                to_beg = std::chrono::steady_clock::now();
                callback_(buffer.get(), ret);
                if (ret == NTRIP_BUFFER_SIZE) continue; // recieve again.
            }

            // Get the current time.
            gga_end = std::chrono::steady_clock::now();

            // Calculate and test the gga interval.
            if (std::chrono::duration_cast<std::chrono::milliseconds>(
                gga_end - gga_beg).count() >= gga_interval_ms) {
                // reset the interval to 0
                gga_beg = std::chrono::steady_clock::now();

                // Check to see if the GGA string requires updating
                if (!gga_update_.load()) {
                    adnav::utils::GenerateGGAString(gga_string_, latitude_,
                        longitude_, altitude_, gnss_fix_, sats_, hdop_);
                    gga_update_.store(true);
                }

                // Send the GGA string
                send(socket_fd_, gga_string_.c_str(), gga_string_.size(), 0);
            }
        }
        log_fn_("NtripClient service done.\r\n");
        service_is_running_.store(false);
    }


    /**
     * @brief Function to take the current server connection loaded
     * into the class and establish a new connection to it. Calling
     * will cause any current connection to be closed before the new
     * connection is opened.
     *
     * @return success boolean.
    */
    bool Client::establish_new_connection(void) {
        stop();

        // Establish a connection with the NTRIPCaster.
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_addr.s_addr = inet_addr(server_ip_.c_str());
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port_);

        #if defined(WIN32) || defined(_WIN32)
            // startup WSA
            WSADATA ws_data;
            if(WSAStartup(MAKEWORD(2, 2), &ws_data) !=0) {
                service_failure_.store(NTRIP_CREATE_SOCK_FAILURE);
                return false;
            }
            socket_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (socket_fd_ == INVALID_SOCKET) {
                log_err_fn_("Winsock Socket Failed!\r\n");
                service_failure_.store(NTRIP_CREATE_SOCK_FAILURE);
                WSACleanup();
                return false;
            }
            server_addr.sin_addr.S_un.S_addr = inet_addr(server_ip_.c_str());
        #else
            socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd_ == -1) {
                log_err_fn_("UNIX Socket Creation Failed!\r\n");
                service_failure_.store(NTRIP_CREATE_SOCK_FAILURE);
                return false;
            }
        #endif // defined(WIN32) || defined(_WIN32)

        // Establish new connection to server.
        if (connect(socket_fd_, reinterpret_cast<struct sockaddr *>(&server_addr),
            sizeof(server_addr)) < 0) {
                log_err_fn_("Connection to NTRIP Caster failed: " + server_ip_ + ":" +
                    std::to_string(server_port_) + " | errno = -" + std::to_string(errno) + " \r\n");
                service_failure_.store(NTRIP_CASTER_CONNECTION_FAILURE);

            #if defined(WIN32) || defined(_WIN32)
                closesocket(socket_fd_);
                WSACleanup();
            #else
                close(socket_fd_);
            #endif // defined(WIN32) || defined(_WIN32)
            return false;
        }

        // Set the sockets to be non-blocking
        #if defined(WIN32) || defined(_WIN32)
            unsigned long ul = 1;

            if(ioctlsocket(socket_fd_, FIONBIO, &ul) == SOCKET_ERROR) {
                closesocket(socket_fd);
                WSACleanup();
                return false;
            }
        #else
            int flags = fcntl(socket_fd_, F_GETFL);
            fcntl(socket_fd_, F_SETFL, flags | O_NONBLOCK);
        #endif // defined(WIN32) || defined(_WIN32)
        return true;
    }

    /**
     * @brief Function to encode the Username and Password using Base
     * 64 Encoding for transmission over HTTP requests.
     *
     * @return encoded string of username:password
    */
    std::string Client::encode_credentials(void) const {
        std::string usr_pwd = user_ + ":" + passwd_;
        std::string encoded;
        adnav::utils::Base64Encode(usr_pwd, &encoded);
        return encoded;
    }
} // namespace adnav::ntrip
