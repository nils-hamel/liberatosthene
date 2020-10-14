/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016-2020 DHLAB, EPFL
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

    # include "eratosthene-server.h"

/*
    source - constructor/destructor methods
 */

    le_server_t le_server_create( le_char_t * const le_path, le_sock_t const le_port ) {

        /* address variables */
        struct sockaddr_in le_addr = LE_ADDRIN_C_PORT( le_port );

        /* created structure variables */
        le_server_t le_server = LE_SERVER_C;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* assign and check server path */
        if ( ( le_server.sv_path = le_path ) == NULL ) {

            /* send message */
            return( le_set_status( le_server, LE_ERROR_IO_ACCESS ) );

        }

        /* check server path consistency */
        if ( le_get_exist( le_server.sv_path ) == _LE_FALSE ) {

            /* send message */
            return( le_set_status( le_server, LE_ERROR_IO_ACCESS ) );

        }

        /* read server configuration */
        if ( ( le_server._status = le_server_set_config( & le_server ) ) != LE_ERROR_SUCCESS ) {

            /* send message */
            return( le_server );

        }

        /* create server socket */
        if ( ( le_server.sv_sock = socket( AF_INET, SOCK_STREAM, 0 ) ) == _LE_SOCK_NULL ) {

            /* send message */
            return( le_set_status( le_server, LE_ERROR_IO_SOCKET ) );

        }

        /* server socket_address */
        if ( bind( le_server.sv_sock, ( struct sockaddr * ) ( & le_addr ), sizeof( struct sockaddr_in ) ) == _LE_SOCK_NULL ) {

            /* push message */
            le_message = LE_ERROR_IO_BIND;

        } else {

            /* server socket state */
            if ( listen( le_server.sv_sock, _LE_USE_PENDING ) == _LE_SOCK_NULL ) {

                /* push message */
                le_message = LE_ERROR_IO_LISTEN;


            } else {

                /* return created structure */
                return( le_server );

            }

        }

        /* close socket */
        close( le_server.sv_sock );

        /* return created structure */
        return( le_set_status( le_server, le_message ) );

    }

    le_void_t le_server_delete( le_server_t * const le_server ) {

        /* deleted structure variables */
        le_server_t le_delete = LE_SERVER_C;

        /* check socket */
        if ( le_server->sv_sock != _LE_SOCK_NULL ) {

            /* close socket */
            close( le_server->sv_sock );

        }

        /* delete structure */
        ( * le_server ) = le_delete;

    }

/*
    source - mutator methods
 */

    le_enum_t le_server_set_config( le_server_t * const le_server ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* stream variable */
        le_file_t le_stream = NULL;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* compose path */
        sprintf( ( char * ) le_path, "%s/system", le_server->sv_path );

        /* create and check stream */
        if ( ( le_stream = fopen( ( char * ) le_path, "r" ) ) == NULL ) {

            /* send message */
            return( LE_ERROR_IO_READ );

        }

        /* read configuration */
        if ( fscanf( le_stream, "%" _LE_SIZE_S, & le_server->sv_scfg ) != 1 ) {

            /* push message */
            le_message = LE_ERROR_IO_READ;

        } else {

            /* read configuration */
            if ( fscanf( le_stream, "%" _LE_TIME_S, & le_server->sv_tcfg ) != 1 ) {

                /* push message */
                le_message = LE_ERROR_IO_READ;

            } else {

                /* check consistency */
                if ( le_server->sv_scfg <= 1 ) {

                    /* push message */
                    le_message = LE_ERROR_DEPTH;

                }

                /* check consistency */
                if ( le_server->sv_scfg >= _LE_USE_DEPTH ) {

                    /* push message */
                    le_message = LE_ERROR_DEPTH;

                }

                /* check consistency */
                if ( le_server->sv_tcfg <= 0 ) {

                    /* push message */
                    le_message = LE_ERROR_TIME;

                }

            }

        }

        /* close stream */
        fclose( le_stream );

        /* send message */
        return( le_message );

    }

/*
    source - i/o methods
 */

    le_void_t le_server_io( le_server_t * const le_server ) {

        /* process variable */
        le_proc_t le_thread;

        /* server principal */
        while ( ( le_server->sv_push = le_client_io_accept( le_server->sv_sock ) ) != _LE_SOCK_NULL ) {

            /* create client process */
            if ( pthread_create( & le_thread, NULL, le_server_io_client, ( le_void_t * ) le_server ) != 0 ) {

                /* reject client connection */
                close( le_server->sv_push );

            } else {

                /* wait thread validation */
                while ( le_server->sv_push != _LE_SOCK_NULL ) {

                    /* thread critical region */
                    pthread_mutex_lock( & le_server->sv_mute );

                    /* thread ciritcal region */
                    pthread_mutex_unlock( & le_server->sv_mute );

                }

            }

        }

    }

    le_void_t * le_server_io_client( le_void_t * le_void ) {

        /* server pointer variable */
        le_server_t * le_server = ( le_server_t * ) le_void;

        /* socket variable */
        le_sock_t le_socket = le_server->sv_push;

        /* switch variable */
        le_switch_t le_switch = LE_SWITCH_C;

        /* mode variable */
        le_enum_t le_mode = LE_MODE_NULL;

        /* stack variable */
        le_array_t le_array[_LE_USE_ARRAY];

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* thread citical region */
        pthread_mutex_lock( & le_server->sv_mute );

        /* send thread validation */
        le_server->sv_push = _LE_SOCK_NULL;

        /* thread critical region */
        pthread_mutex_unlock( & le_server->sv_mute );

        /* create socket-array stack */
        le_array_mac_create( le_array, _LE_USE_ARRAY );

        /* create switch */
        le_switch = le_switch_create( le_server->sv_path, le_server->sv_scfg, le_server->sv_tcfg );

        /* client connection */
        while ( le_message == LE_ERROR_SUCCESS ) {

            /* wait client socket-array */
            if ( ( le_mode = le_array_io_read( le_array, le_socket ) ) == LE_MODE_NULL ) {

                /* push message */
                le_message = LE_ERROR_IO_ARRAY;

            } else {

                /* switch reload management */
                if ( ( le_message = le_switch_set_update( & le_switch, 5 ) ) == LE_ERROR_SUCCESS ) {

                    /* switch on socket-array mode */
                    switch ( le_mode ) {

                        case ( LE_MODE_AUTH ) : {

                            /* service management */
                            le_message = le_switch_io_auth( & le_switch, le_array, le_socket );

                        } break;

                        case ( LE_MODE_INJE ) : {

                            /* service management */
                            le_message = le_switch_io_inject( & le_switch, le_array, le_socket );

                        } break;

                        case ( LE_MODE_DETE ) : {

                            /* service management */
                            le_message = le_switch_io_detect( & le_switch, le_array, le_socket );

                        } break;

                        case ( LE_MODE_QUER ) : {

                            /* service management */
                            le_message = le_switch_io_query( & le_switch, le_array, le_socket );

                        } break;

                        default : {

                            /* push message */
                            le_message = LE_ERROR_IO_ARRAY;

                        } break;

                    }

                }

            }

        }

        /* close socket */
        close( le_socket );

        /* delete switch */
        le_switch_delete( & le_switch );

        /* delete socket-array stack */
        le_array_mac_delete( le_array, _LE_USE_ARRAY );

        /* return null pointer */
        return( NULL );

    }

