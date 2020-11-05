/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@alumni.epfl.ch
 *      Copyright (c) 2016-2020 DHLAB, EPFL
 *      Copyright (c) 2020 Republic and Canton of Geneva
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

    # include "eratosthene-client.h"

/*
    source - constructor/destructor methods
 */

    le_sock_t le_client_create( le_char_t const * const le_ip, le_sock_t const le_port ) {

        /* socket option variables */
        int le_option = 1;

        /* address variables */
        struct sockaddr_in le_addr = LE_ADDRIN_C_PORT( le_port );

        /* returned value variables */
        le_sock_t le_socket = _LE_SOCK_NULL;

        /* check consistency */
        if ( le_ip == NULL ) {

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* convert address */
        if ( inet_pton( AF_INET, ( char * ) le_ip, & le_addr.sin_addr ) <= 0 ) {

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* create socket */
        if ( ( le_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) == _LE_SOCK_NULL ) {

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* socket option */
        if ( setsockopt( le_socket, IPPROTO_TCP, TCP_NODELAY, & le_option, sizeof( int ) ) == _LE_SOCK_NULL ) {

            /* close socket */
            close( le_socket );

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* socket connection */
        if ( connect( le_socket, ( struct sockaddr * ) & le_addr, sizeof( struct sockaddr_in ) ) == _LE_SOCK_NULL ) {

            /* close socket */
            close( le_socket );

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* return socket */
        return( le_socket );

    }

    le_sock_t le_client_delete( le_sock_t const le_socket ) {

        /* check socket */
        if ( le_socket != _LE_SOCK_NULL ) {

            /* close socket */
            close( le_socket );

        }

        /* return null socket */
        return( _LE_SOCK_NULL );

    }

/*
    source - display methods
 */

    le_void_t le_client_print_host( le_sock_t const le_socket, le_file_t const le_stream ) {

        /* host address */
        struct sockaddr_in le_addr = LE_ADDRIN_C;

        /* address structure length */
        socklen_t le_length = sizeof( struct sockaddr_in );

        /* retreive host address */
        getpeername( le_socket, ( struct sockaddr * ) & le_addr, & le_length );

        /* display host address in stream */
        fprintf( le_stream, "%s\n", inet_ntoa( le_addr.sin_addr ) );

    }

/*
    source - i/o methods
 */

    le_sock_t le_client_io_accept( le_sock_t const le_listen ) {

        /* socket option variables */
        int le_option = 1;

        /* client address variables */
        struct sockaddr_in le_addr = LE_ADDRIN_C;

        /* client address variables */
        socklen_t le_length = sizeof( struct sockaddr_in );

        /* returned value variables */
        le_sock_t le_socket = _LE_SOCK_NULL;

        /* accept client connection */
        if ( ( le_socket = accept( le_listen, ( struct sockaddr * ) & le_addr, & le_length ) ) == _LE_SOCK_NULL ) {

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* socket option */
        if ( setsockopt( le_socket, IPPROTO_TCP, TCP_NODELAY, & le_option, sizeof( int ) ) == _LE_SOCK_NULL ) {

            /* close socket */
            close( le_socket );

            /* send message */
            return( _LE_SOCK_NULL );

        }

        /* return socket */
        return( le_socket );

    }

