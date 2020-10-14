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

    # include "eratosthene-door.h"

/*
    source - constructor/destructor methods
 */

    le_door_t le_door_create( le_char_t const * const le_root, le_size_t const le_scfg, le_time_t const le_tcfg, le_time_t const le_time, le_enum_t const le_mode ) {

        /* created structure variable */
        le_door_t le_door = LE_DOOR_C_SCT( le_scfg, le_tcfg, le_time / le_tcfg );

        /* compute path */
        sprintf( ( char * ) le_door.dr_path, "%s/%" _LE_TIME_P "/", le_root, le_door.dr_time );

        /* compute path length */
        le_door.dr_plen = strlen( ( char * ) le_door.dr_path );

        /* create path copy for stream management */
        strcpy( ( char * ) le_door.dr_strp, ( char * ) le_door.dr_path );

        /* check mode */
        if ( le_mode == LE_DOOR_WRITE ) {

            /* check door implementation */
            if ( le_get_exist( le_door.dr_path ) == _LE_TRUE ) {

                /* return created structure */
                return( le_set_status( le_door, LE_ERROR_IO_WRITE ) );

            } else {

                /* bootstrap door implementation */
                if ( le_door_set_bootstrap( & le_door ) == _LE_FALSE ) {

                    /* return created structure */
                    return( le_set_status( le_door, LE_ERROR_IO_WRITE ) );

                }

            }

        }

        /* return created structure */
        return( le_door );

    }

    le_void_t le_door_delete( le_door_t * const le_door ) {

        /* deleted structure variable */
        le_door_t le_delete = LE_DOOR_C;

        /* delete structure */
        ( * le_door ) = le_delete;

    }

/*
    source - accessor methods
 */

    le_time_t le_door_get_reduced( le_door_t const * const le_door ) {

        /* return door reduced time */
        return( le_door->dr_time );

    }

    le_enum_t le_door_get_nearest( le_door_t const * const le_prev, le_door_t const * const le_next, le_time_t const le_reduced ) {

        /* differential variable */
        le_time_t le_pdiff = _LE_TIME_NULL;

        /* differential variable */
        le_time_t le_ndiff = _LE_TIME_NULL;

        /* check link */
        if ( le_prev->dr_prev == NULL ) {

            /* send message */
            return( LE_DOOR_NEXT );

        }

        /* check link */
        if ( le_next->dr_next == NULL ) {

            /* send message */
            return( LE_DOOR_PREV );

        }

        /* compute differential */
        le_pdiff = ( ( le_door_t * ) le_prev->dr_prev )->dr_time - le_reduced;

        /* compute differential */
        le_ndiff = ( ( le_door_t * ) le_next->dr_next )->dr_time - le_reduced;

        /* compare differential */
        if ( le_time_abs( le_pdiff ) < le_time_abs( le_ndiff ) ) {

            /* send message */
            return( LE_DOOR_PREV );

        } else {

            /* send message */
            return( LE_DOOR_NEXT );

        }

    }

    le_enum_t le_door_get_mono( le_door_t const * const le_door ) {

        /* check pointer */
        if ( le_door != NULL ) {

            /* check offset */
            if ( le_door->dr_moff != _LE_OFFS_NULL ) {

                /* send message */
                return( _LE_TRUE );

            } else {

                /* send message */
                return( _LE_FALSE );

            }

        } else {

            /* send message */
            return( _LE_FALSE );

        }

    }

    le_enum_t le_door_get_poly( le_door_t const * const le_door ) {

        /* check pointer */
        if ( le_door != NULL ) {

            /* check offset */
            if ( le_door->dr_poff != _LE_OFFS_NULL ) {

                /* send message */
                return( _LE_TRUE );

            } else {

                /* send message */
                return( _LE_FALSE );

            }

        } else {

            /* send message */
            return( _LE_FALSE );

        }

    }

    le_enum_t le_door_get_equal( le_door_t const * const le_door, le_time_t const le_time ) {

        /* compare time and send message */
        return( ( le_door->dr_time == ( le_time / le_door->dr_tcfg ) ) ? _LE_TRUE : _LE_FALSE );

    }

    le_enum_t le_door_get_anterior( le_door_t const * const le_door, le_door_t const * const le_candidate ) {

        /* compare time and send message */
        return( ( le_candidate->dr_time < le_door->dr_time ) ? _LE_TRUE : _LE_FALSE );

    }

    le_door_t * le_door_get_prev( le_door_t const * const le_door ) {

        /* send door link */
        return( ( le_door_t * ) le_door->dr_prev );

    }

    le_door_t * le_door_get_next( le_door_t const * const le_door ) {

        /* send door link */
        return( ( le_door_t * ) le_door->dr_next );

    }

    le_enum_t le_door_get_dispatch( le_door_t const * const le_door, le_size_t const le_suffix ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* compose path */
        sprintf( ( char * ) le_path, "%s0/%" _LE_SIZE_P "0", le_door->dr_path, le_suffix );

        /* check chunk */
        return( le_get_exist( le_path ) );

    }

/*
    source - mutator methods
 */

    le_enum_t le_door_set_bootstrap( le_door_t * const le_door ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* stream variable */
        le_file_t le_stream = NULL;

        /* check door implementation */
        if ( le_get_exist( le_door->dr_path ) == _LE_TRUE ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* create directory */
        if ( mkdir( ( char * ) le_door->dr_path, 0755 ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* compose path */
        sprintf( ( char * ) le_path, "%s0", le_door->dr_path );

        /* create directory */
        if ( mkdir( ( char * ) le_path, 0755 ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* compose path */
        sprintf( ( char * ) le_path, "%s1", le_door->dr_path );

        /* create directory */
        if ( mkdir( ( char * ) le_path, 0755 ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* compose path */
        sprintf( ( char * ) le_path, "%s2", le_door->dr_path );

        /* create directory */
        if ( mkdir( ( char * ) le_path, 0755 ) != 0 ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* parsing each-vertex stream */
        for ( le_size_t le_parse = 0; le_parse < le_door->dr_scfg; le_parse ++ ) {

            /* compose path */
            sprintf( ( char * ) le_path, "%s1/%02" _LE_SIZE_P, le_door->dr_path, le_parse );

            /* create and check stream */
            if ( ( le_stream = fopen( ( char * ) le_path, "w" ) ) == NULL ) {

                /* send message */
                return( _LE_FALSE );

            /* delete stream */
            } else { fclose( le_stream ); }

            /* compose path */
            sprintf( ( char * ) le_path, "%s2/%02" _LE_SIZE_P, le_door->dr_path, le_parse );

            /* create and check stream */
            if ( ( le_stream = fopen( ( char * ) le_path, "w" ) ) == NULL ) {

                /* send message */
                return( _LE_FALSE );

            /* delete stream */
            } else { fclose( le_stream ); }

        }

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_door_set_stream( le_door_t * const le_door, le_enum_t const le_mode ) {

        /* check door */
        if ( le_door == NULL ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* check state */
        if ( le_door->dr_mode == le_mode ) {

            /* send message */
            return( _LE_TRUE );

        }

        /* check state */
        if ( le_mode == LE_DOOR_CLOSE ) {

            /* parsing each-vertex stream */
            for ( le_size_t le_parse = 0; le_parse < le_door->dr_scfg; le_parse ++ ) {

                /* check stream */
                if ( le_door->dr_macc[le_parse] != NULL ) {

                    /* delete stream */
                    fclose( le_door->dr_macc[le_parse] );

                }

                /* check stream */
                if ( le_door->dr_pacc[le_parse] != NULL ) {

                    /* delete stream */
                    fclose( le_door->dr_pacc[le_parse] );

                }

            }

            /* check stream - poly-vertex specific */
            if ( le_door->dr_pdat != NULL ) {

                /* delete stream */
                fclose( le_door->dr_pdat );

            }

            /* update door state */
            le_door->dr_mode = LE_DOOR_CLOSE;

            /* send message */
            return( _LE_TRUE );

        } else {

            /* path tail specification */
            le_door_mac_tail( le_door, 0, '2' );
            le_door_mac_tail( le_door, 1, '_' );

            /* path tail termination */
            le_door_mac_tail( le_door, 2, 0 );

            /* create stream */
            le_door->dr_pdat = fopen( ( char * ) le_door->dr_strp, "r+" );

            /* parsing each vertex stream */
            for ( le_size_t le_parse = 0; le_parse < le_door->dr_scfg; le_parse ++ ) {

                /* path tail specification */
                le_door_mac_tail( le_door, 0, '1' );
                le_door_mac_tail( le_door, 1, '/' );

                /* path tail specification - scale */
                le_door_mac_tail( le_door, 2, ( le_parse / 10 ) + 48 );
                le_door_mac_tail( le_door, 3, ( le_parse % 10 ) + 48 );

                /* create and check stream */
                if ( ( le_door->dr_macc[le_parse] = fopen( ( char * ) le_door->dr_strp, "r+" ) ) == NULL ) {

                    /* delete already created streams */
                    le_door_set_stream( le_door, LE_DOOR_CLOSE );

                    /* send message */
                    return( _LE_FALSE );

                }

                /* path tail specification */
                le_door_mac_tail( le_door, 0, '2' );

                /* create and check stream */
                if ( ( le_door->dr_pacc[le_parse] = fopen( ( char * ) le_door->dr_strp, "r+" ) ) == NULL ) {

                    /* delete already created streams */
                    le_door_set_stream( le_door, LE_DOOR_CLOSE );

                    /* send message */
                    return( _LE_FALSE );

                }

            }

            /* update door state */
            le_door->dr_mode = LE_DOOR_OPEN;

            /* send message */
            return( _LE_TRUE );

        }

    }

    le_enum_t le_door_set_reset( le_door_t * const le_door, le_size_t const le_type ) {

        /* stream stack variable */
        le_file_t * le_stack = NULL;

        /* check door state */
        if ( le_door->dr_mode != LE_DOOR_OPEN ) {

            /* send message */
            return( _LE_FALSE );

        }

        /* check mode */
        if ( le_type == 1 ) {

            /* assign desired stream stack */
            le_stack = le_door->dr_macc;

        } else {

            /* assign desired stream stack */
            le_stack = le_door->dr_pacc;

        }

        /* parsing stream stack */
        for ( le_size_t le_parse = 0; le_parse < le_door->dr_scfg; le_parse ++ ) {

            /* reset stream offset */
            fseek( le_stack[le_parse], 0, SEEK_SET );

            /* reset stream content */
            ftruncate( fileno( le_stack[le_parse] ), 0 );

        }

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_door_set_state( le_door_t const * const le_door, le_enum_t const le_state ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* stream variable */
        le_file_t le_stream = NULL;

        /* compose path */
        sprintf( ( char * ) le_path, "%slock", le_door->dr_path );

        /* check target state */
        if ( le_state == LE_DOOR_LOCK ) {

            /* check current state */
            if ( le_get_exist( le_path ) == _LE_TRUE ) {

                /* send message */
                return( _LE_FALSE );

            } else {

                /* create and check stream */
                if ( ( le_stream = fopen( ( char * ) le_path, "w" ) ) == NULL ) {

                    /* send message */
                    return( _LE_FALSE );

                } else {

                    /* delete stream */
                    fclose( le_stream );

                    /* send message */
                    return( _LE_TRUE );

                }

            }

        } else {

            /* check current state */
            if ( le_get_exist( le_path ) == _LE_FALSE ) {

                /* send message */
                return( _LE_FALSE );

            } else {

                /* update state */
                if ( remove( ( char * ) le_path ) != 0 ) {

                    /* send message */
                    return( _LE_FALSE );

                } else {

                    /* send message */
                    return( _LE_TRUE );

                }

            }

        }

    }

    le_void_t le_door_set_insert( le_door_t * const le_door, le_door_t * const le_prev, le_door_t * const le_next ) {

        /* update chain link */
        le_door->dr_prev = ( le_void_t * ) le_prev;

        /* update chain link */
        le_door->dr_next = ( le_void_t * ) le_next;

        /* check pointer */
        if ( le_prev != NULL ) {

            /* update chain link */
            le_prev->dr_next = ( le_void_t * ) le_door;

        }

        /* check pointer */
        if ( le_next != NULL ) {

            /* update chain link */
            le_next->dr_prev = ( le_void_t * ) le_door;

        }

    }

/*
    source - i/o methods
 */

    le_enum_t le_door_io_each_inject_dispatch( le_door_t const * const le_door, le_array_t * const le_array, le_sock_t const le_socket ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* stream variable */
        le_file_t le_stream = NULL;

        /* mode variable */
        le_enum_t le_mode = LE_MODE_INJE;

        /* stack variable */
        le_size_t le_stack = 1;

        /* dispatch variable */
        le_size_t le_switch = 0;

        /* array pointer variable */
        le_byte_t * le_head = NULL;
        le_byte_t * le_edge = NULL;

        /* buffer pointer variable */
        le_byte_t * le_mbyte = NULL;
        le_byte_t * le_pbyte = NULL;

        /* index variable */
        le_size_t le_mindex = 0;
        le_size_t le_pindex = 0;

        /* stack variable */
        le_size_t le_mstack = 0;
        le_size_t le_pstack = 0;

        /* size variable */
        le_size_t le_chunk = LE_ARRAY_DATA * LE_UV3_CHUNK;

        /* size variable */
        le_size_t le_spare = LE_ARRAY_DATA * _LE_BYTE_MAX + le_chunk;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* allocate buffer memory */
        if ( ( le_mbyte = ( le_byte_t * ) malloc( le_chunk ) ) == NULL ) {

            /* push message */
            le_message = LE_ERROR_MEMORY;

        } else {

            /* allocate buffer memory */
            if ( ( le_pbyte = ( le_byte_t * ) malloc( le_spare ) ) == NULL ) {

                /* push message */
                le_message = LE_ERROR_MEMORY;

            } else {

                /* array stream dispatch */
                while ( ( le_mode == LE_MODE_INJE ) && ( le_message == LE_ERROR_SUCCESS ) ) {

                    /* exhaust condition */
                    if ( le_head == le_edge ) {

                        /* read and check next array */
                        if ( ( le_mode = le_array_io_read( le_array, le_socket ) ) == LE_MODE_INJE ) {

                            /* compute array pointer */
                            le_edge = ( le_head = le_array_get_byte( le_array ) ) + le_array_get_size( le_array );

                        } else {

                            /* assign array pointer */
                            le_edge = ( le_head = NULL );

                        }

                    }

                    /* stack management */
                    le_stack --;

                    /* check index */
                    if ( ( le_stack == 0 ) && ( ( le_mindex >= le_chunk ) || ( ( le_head == NULL ) && ( le_mindex != 0 ) ) ) ) {

                        /* sort chunk */
                        if ( ( le_mbyte = le_uv3_set_sort( le_mbyte, le_mindex, le_chunk, le_door->dr_scfg ) ) == NULL ) {

                            /* push message */
                            le_message = LE_ERROR_MEMORY;

                        } else {

                            /* compose path */
                            sprintf( ( char * ) le_path, "%s0/1%" _LE_SIZE_P, le_door->dr_path, le_mstack ++ );

                            /* create and check stream */
                            if ( ( le_stream = fopen( ( char * ) le_path, "wb" ) ) == NULL ) {

                                /* push message */
                                le_message = LE_ERROR_IO_WRITE;

                            } else {

                                /* export chunk */
                                if ( fwrite( le_mbyte, sizeof( le_byte_t ), le_mindex, le_stream ) != le_mindex ) {

                                    /* push message */
                                    le_message = LE_ERROR_IO_WRITE;

                                } else {

                                    /* reset index */
                                    le_mindex = 0;

                                }

                                /* delete stream */
                                fclose( le_stream );

                            }

                        }

                    }

                    /* check index */
                    if ( ( le_stack == 0 ) && ( ( le_pindex >= le_chunk ) || ( ( le_head == NULL ) && ( le_pindex != 0 ) ) ) ) {

                        /* sort chunk */
                        if ( ( le_pbyte = le_uv3_set_sort( le_pbyte, le_pindex, le_spare, le_door->dr_scfg ) ) == NULL ) {

                            /* push message */
                            le_message = LE_ERROR_MEMORY;

                        } else {

                            /* compose path */
                            sprintf( ( char * ) le_path, "%s0/2%" _LE_SIZE_P, le_door->dr_path, le_pstack ++ );

                            /* create and check stream */
                            if ( ( le_stream = fopen( ( char * ) le_path, "wb" ) ) == NULL ) {

                                /* push message */
                                le_message = LE_ERROR_IO_WRITE;

                            } else {

                                /* export chunk */
                                if ( fwrite( le_pbyte, sizeof( le_byte_t ), le_pindex, le_stream ) != le_pindex ) {

                                    /* push message */
                                    le_message = LE_ERROR_IO_WRITE;

                                } else {

                                    /* reset index */
                                    le_pindex = 0;

                                }

                                /* delete stream */
                                fclose( le_stream );

                            }

                        }

                    }

                    /* check array head */
                    if ( le_head != NULL ) {

                        /* check stack state */
                        if ( le_stack == 0 ) {

                            /* update stack */
                            le_stack = ( le_switch = le_uv3_get_type( le_head ) );

                        }

                        /* check dispatch */
                        if ( le_switch == 1 ) {

                            /* dispatch record */
                            memcpy( ( char * ) ( le_mbyte + le_mindex ), ( char * ) le_head, LE_ARRAY_DATA );

                            /* update index */
                            le_mindex += LE_ARRAY_DATA;

                        } else {

                            /* dispatch record */
                            memcpy( ( char * ) ( le_pbyte + le_pindex ), ( char * ) le_head, LE_ARRAY_DATA );

                            /* update index */
                            le_pindex += LE_ARRAY_DATA;

                        }

                        /* update head */
                        le_head += LE_ARRAY_DATA;

                    }

                }

                /* release buffer memory */
                free( le_pbyte );

            }

            /* release buffer memory */
            free( le_mbyte );

        }

        /* send message */
        return( le_message );

    }

    le_enum_t le_door_io_each_inject_merge( le_door_t const * const le_door, le_size_t const le_suffix ) {

        /* path variable */
        le_char_t le_pchunk[_LE_USE_PATH] = { 0 };

        /* path variable */
        le_char_t le_qchunk[_LE_USE_PATH] = { 0 };

        /* path variable */
        le_char_t le_derive[_LE_USE_PATH] = { 0 };

        /* merge variable */
        le_size_t le_merge = 2;

        /* index variable */
        le_size_t le_index = 0;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* merge process */
        while ( ( le_merge > 1 ) && ( le_message == LE_ERROR_SUCCESS ) ) {

            /* reset merge */
            le_merge = 0;

            /* reset index */
            le_index = 1;

            /* parsing chunk pairs */
            while ( ( le_index != 0 ) && ( le_message == LE_ERROR_SUCCESS ) ) {

                /* compose chunk path */
                sprintf( ( char * ) le_pchunk, "%s0/%" _LE_SIZE_P "%" _LE_SIZE_P, le_door->dr_path, le_suffix, le_index - 1 );

                /* compose chunk path */
                sprintf( ( char * ) le_qchunk, "%s0/%" _LE_SIZE_P "%" _LE_SIZE_P, le_door->dr_path, le_suffix, le_index );

                /* check chunk */
                if ( le_get_exist( le_qchunk ) == _LE_TRUE ) {

                    /* check chunk */
                    if ( le_get_exist( le_pchunk ) == _LE_TRUE ) {

                        /* compose derivation path */
                        sprintf( ( char * ) le_derive, "%s_", le_pchunk );

                        /* merge chunks */
                        if ( ( le_message = le_uv3_set_merge( le_pchunk, le_qchunk, le_derive, le_door->dr_scfg ) ) == LE_ERROR_SUCCESS ) {

                            /* remove previous chunk */
                            if ( ( remove( ( char * ) le_pchunk ) | remove( ( char * ) le_qchunk ) ) != 0 ) {

                                /* push message */
                                le_message = LE_ERROR_IO_ACCESS;

                            } else {

                                /* compose next path */
                                sprintf( ( char * ) le_pchunk, "%s0/%" _LE_SIZE_P "%" _LE_SIZE_P, le_door->dr_path, le_suffix, le_index >> 1 );

                                /* move chunk */
                                if ( rename( ( char * ) le_derive, ( char * ) le_pchunk ) != 0 ) {

                                    /* push message */
                                    le_message = LE_ERROR_IO_ACCESS;

                                } else {

                                    /* update index */
                                    le_index += 2;

                                    /* update merge */
                                    le_merge ++;

                                }

                            }

                        }

                    } else {

                        /* push message */
                        le_message = LE_ERROR_IO_ACCESS;

                    }

                } else {

                    /* check chunk */
                    if ( le_get_exist( le_pchunk ) == _LE_TRUE ) {

                        /* compose next path */
                        sprintf( ( char * ) le_derive, "%s0/%" _LE_SIZE_P "%" _LE_SIZE_P, le_door->dr_path, le_suffix, le_index >> 1 );

                        /* avoid single-chunk dispatch */
                        if ( le_index > 1 ) {

                            /* move chunk */
                            if ( rename( ( char * ) le_pchunk, ( char * ) le_derive ) != 0 ) {

                                /* push message */
                                le_message = LE_ERROR_IO_ACCESS;

                            }

                        }

                        /* update merge */
                        le_merge ++;

                    }

                    /* update index */
                    le_index = 0;

                }

            }

        }

        /* compose chunk path */
        sprintf( ( char * ) le_pchunk, "%s0/%" _LE_SIZE_P "0", le_door->dr_path, le_suffix );

        /* compose chunk path */
        sprintf( ( char * ) le_qchunk, "%s%" _LE_SIZE_P "_", le_door->dr_path, le_suffix );

        /* check state */
        if ( le_get_exist( le_pchunk ) == _LE_TRUE ) {

            /* check persistant chunk */
            if ( le_get_exist( le_qchunk ) == _LE_TRUE ) {

                /* compose derivation path */
                sprintf( ( char * ) le_derive, "%s0/%" _LE_SIZE_P "1", le_door->dr_path,le_suffix );

                /* merge stream with persistant */
                if ( ( le_message = le_uv3_set_merge( le_pchunk, le_qchunk, le_derive, le_door->dr_scfg ) ) == LE_ERROR_SUCCESS ) {

                    /* remove chunks */
                    if ( ( remove( ( char * ) le_pchunk ) | remove( ( char * ) le_qchunk ) ) != 0 ) {

                        /* push message */
                        le_message = LE_ERROR_IO_ACCESS;

                    } else {

                        /* update persistant chunk */
                        if ( rename( ( char * ) le_derive, ( char * ) le_qchunk ) != 0 ) {

                            /* push message */
                            le_message = LE_ERROR_IO_ACCESS;

                        }

                    }

                }

            } else {

                /* compose chunk path */
                sprintf( ( char * ) le_pchunk, "%s0/%" _LE_SIZE_P "0", le_door->dr_path, le_suffix );

                /* update persistant chunk */
                if ( rename( ( char * ) le_pchunk, ( char * ) le_qchunk ) != 0 ) {

                    /* push message */
                    le_message = LE_ERROR_IO_ACCESS;

                }

            }

        }

        /* send message */
        return( le_message );

    }

    le_enum_t le_door_io_mono_inject( le_door_t const * const le_door ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* address variable */
        le_address_t le_addr = LE_ADDRESS_C_SIZE( le_door->dr_scfg );

        /* address variable */
        le_address_t le_hold = LE_ADDRESS_C_SIZE( le_door->dr_scfg );

        /* class variable */
        le_mono_t le_class[_LE_USE_DEPTH];

        /* offset variable */
        le_size_t le_offset[_LE_USE_DEPTH] = { 0 };

        /* buffer variable */
        le_byte_t * le_buffer = NULL;

        /* size variable */
        le_size_t le_size = LE_UV3_CHUNK * LE_ARRAY_DATA;

        /* stream variable */
        le_file_t le_stream = NULL;

        /* reading variable */
        le_size_t le_read = 0;

        /* parsing variable */
        le_size_t le_parse = 0;

        /* flag variable */
        le_enum_t le_first = _LE_FALSE;

        /* flag variable */
        le_enum_t le_flag = _LE_FALSE;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* compose path */
        sprintf( ( char * ) le_path, "%s1_", le_door->dr_path );

        /* create stream */
        if ( ( le_stream = fopen( ( char * ) le_path, "rb" ) ) == NULL ) {

            /* push message */
            le_message = LE_ERROR_IO_READ;

        } else {

            /* allocate buffer memory */
            if ( ( le_buffer = ( le_byte_t * ) malloc( le_size ) ) == NULL ) {

                /* push message */
                le_message = LE_ERROR_MEMORY;

            } else {

                /* classes stack initialisation */
                for ( le_size_t le_depth = 0; le_depth < le_door->dr_scfg; le_depth ++ ) {

                    /* initialise class */
                    le_class[le_depth] = le_mono_create();

                }

                /* stream chunk reading */
                while ( ( le_read = fread( le_buffer, sizeof( le_byte_t ), le_size, le_stream ) ) > 0 ) {

                    /* reset parser */
                    le_parse = 0;

                    /* parsing stream chunk */
                    while ( le_parse < le_read ) {

                        /* hold address */
                        le_hold = le_addr;

                        /* compute address */
                        le_address_set_pose( & le_addr, ( le_real_t * ) ( le_buffer + le_parse ) );

                        /* wait primitive */
                        if ( le_first == _LE_TRUE ) {

                            /* reset continuous exportation */
                            le_flag = _LE_FALSE;

                            /* class transversal exportation */
                            for ( le_size_t le_depth = 1; le_depth < le_door->dr_scfg; le_depth ++ ) {

                                /* check continuous exportation */
                                if ( le_flag == _LE_FALSE ) {

                                    /* compare address digit */
                                    if ( le_address_get_digit( & le_addr, le_depth - 1 ) != le_address_get_digit( & le_hold, le_depth - 1 ) ) {

                                        /* update continuous exportation */
                                        le_flag = _LE_TRUE;

                                    }

                                }

                                /* check continous exportation */
                                if ( le_flag == _LE_TRUE ) {

                                    /* class exportation */
                                    le_mono_io_write( le_class + le_depth, _LE_OFFS_NULL, le_door->dr_macc[le_depth] );

                                    /* reset class */
                                    le_mono_reset( le_class + le_depth );

                                    /* update offset */
                                    le_offset[le_depth] = ftell( le_door->dr_macc[le_depth] );

                                }

                            }

                        }

                        /* injection and offset assignation */
                        for ( le_size_t le_depth = 0; le_depth < le_door->dr_scfg - 1; le_depth ++ ) {

                            /* update class offset */
                            le_mono_set_offset( le_class + le_depth, le_address_get_digit( & le_addr, le_depth ), le_offset[le_depth + 1] );

                            /* push primitive */
                            le_mono_set_push( le_class + le_depth, ( le_byte_t * ) ( le_buffer + le_parse + LE_ARRAY_DATA_POSE + LE_ARRAY_DATA_TYPE ) );

                        }

                        /* push primitive */
                        le_mono_set_push( le_class + le_door->dr_scfg - 1, ( le_byte_t * ) ( le_buffer + le_parse + LE_ARRAY_DATA_POSE + LE_ARRAY_DATA_TYPE ) );

                        /* update parser */
                        le_parse += LE_ARRAY_DATA;

                        /* update flag */
                        le_first = _LE_TRUE;

                    }

                }

                /* terminal transversal exportation */
                for ( le_size_t le_depth = 0; le_depth < le_door->dr_scfg; le_depth ++ ) {

                    /* class exportation */
                    le_mono_io_write( le_class + le_depth, _LE_OFFS_NULL, le_door->dr_macc[le_depth] );

                    /* delete class */
                    le_mono_delete( le_class + le_depth );

                }

                /* release buffer memory */
                free( le_buffer );

            }

            /* delete stream */
            fclose( le_stream );

        }

        /* send message */
        return( le_message );

    }

    le_enum_t le_door_io_poly_inject( le_door_t const * const le_door ) {

        /* path variable */
        le_char_t le_path[_LE_USE_PATH] = { 0 };

        /* pose variable */
        le_real_t le_origin[3] = { 0. };

        /* address variable */
        le_address_t le_addr = LE_ADDRESS_C_SIZE( le_door->dr_scfg );

        /* address variable */
        le_address_t le_hold = LE_ADDRESS_C_SIZE( le_door->dr_scfg );

        /* class variable */
        le_poly_t le_class[_LE_USE_DEPTH];

        /* offset variable */
        le_size_t le_offset[_LE_USE_DEPTH] = { 0 };

        /* buffer variable */
        le_byte_t * le_buffer = NULL;

        /* size variable */
        le_size_t le_size = LE_UV3_CHUNK * LE_ARRAY_DATA;

        /* stream variable */
        le_file_t le_stream = NULL;

        /* reading variable */
        le_size_t le_read = 0;

        /* parsing variable */
        le_size_t le_parse = 0;

        /* parsing variable */
        le_size_t le_index = 0;

        /* offset variable */
        le_size_t le_master = 0;

        /* stack variable */
        le_size_t le_stack = 0;

        /* depth variable */
        le_size_t le_inject = 0;

        /* distance variable */
        le_size_t le_split = 0;

        /* flag variable */
        le_enum_t le_flag = _LE_FALSE;

        /* message variable */
        le_enum_t le_message = LE_ERROR_SUCCESS;

        /* compose path */
        sprintf( ( char * ) le_path, "%s2_", le_door->dr_path );

        /* create stream */
        if ( ( le_stream = fopen( ( char * ) le_path, "rb" ) ) == NULL ) {

            /* push message */
            le_message = LE_ERROR_IO_READ;

        } else {

            /* allocate buffer memory */
            if ( ( le_buffer = ( le_byte_t * ) malloc( le_size ) ) == NULL ) {

                /* push message */
                le_message = LE_ERROR_MEMORY;

            } else {

                /* classes stack initialisation */
                for ( le_size_t le_depth = 0; le_depth < le_door->dr_scfg; le_depth ++ ) {

                    /* initialise class */
                    le_class[le_depth] = le_poly_create();

                }

                /* stream chunk reading */
                while ( ( le_read = fread( le_buffer, sizeof( le_byte_t ), le_size, le_stream ) ) > 0 ) {

                    /* reset parser */
                    le_parse = 0;

                    /* stream chunk parsing */
                    while ( le_parse < le_read ) {

                        /* primitive detection */
                        if ( le_stack == 0 ) {

                            /* hold address */
                            le_hold = le_addr;

                            /* push initial vertex */
                            le_uv3_get_record_pose( le_buffer + le_parse, le_origin );

                            /* compute address */
                            le_address_set_pose( & le_addr, ( le_real_t * ) ( le_buffer + le_parse ) );

                            /* wait for initial injection */
                            if ( le_index > 0 ) {

                                /* reset continuous exportation */
                                le_flag = _LE_FALSE;

                                /* class transversal exportation */
                                for ( le_size_t le_depth = 1; le_depth < le_door->dr_scfg; le_depth ++ ) {

                                    /* check continuous exportation */
                                    if ( le_flag == _LE_FALSE ) {

                                        /* compare address digit */
                                        if ( le_address_get_digit( & le_addr, le_depth - 1 ) != le_address_get_digit( & le_hold, le_depth - 1 ) ) {

                                            /* update continuous exportation */
                                            le_flag = _LE_TRUE;

                                        }

                                    }

                                    /* check continuous exportation */
                                    if ( le_flag == _LE_TRUE ) {

                                        /* class exportation */
                                        le_poly_io_write( le_class + le_depth, _LE_OFFS_NULL, le_door->dr_pacc[le_depth] );

                                        /* reset class */
                                        le_poly_reset( le_class + le_depth );

                                        /* update offset */
                                        le_offset[le_depth] = ftell( le_door->dr_pacc[le_depth] );

                                    }

                                }

                            }

                            /* update primitive stack */
                            le_stack = le_uv3_get_type( le_buffer + le_parse );

                            /* primitive master index */
                            le_master = le_index;

                            /* reset injection depth */
                            le_inject = le_door->dr_scfg;

                        } else {

                            /* compute and check distance */
                            if ( ( le_split = le_geodesy_get_optimal_scale( ( le_real_t * ) ( le_buffer + le_parse ), le_origin ) ) < le_inject ) {

                                /* update injection depth */
                                le_inject = le_split;

                            }

                        }

                        /* anticipated primitive detection */
                        if ( ( -- le_stack ) == 0 ) {

                            /* primitive injection and offset assignation */
                            for ( le_size_t le_depth = 0; le_depth < le_door->dr_scfg - 1; le_depth ++ ) {

                                /* update class offset */
                                le_poly_set_offset( le_class + le_depth, le_address_get_digit( & le_addr, le_depth ), le_offset[le_depth + 1] );

                                /* check injection depth */
                                if ( le_depth >= le_inject ) {

                                    /* push primitive */
                                    le_poly_set_push( le_class + le_depth, le_master );

                                }

                            }

                            /* forced last injection */
                            le_poly_set_push( le_class + le_door->dr_scfg - 1, le_master );

                        }

                        /* update parser */
                        le_parse += LE_ARRAY_DATA;

                        /* update offset */
                        le_index += LE_ARRAY_DATA;

                    }

                }

                /* terminal transversal exportation */
                for ( le_size_t le_depth = 0; le_depth < le_door->dr_scfg; le_depth ++ ) {

                    /* class exportation */
                    le_poly_io_write( le_class + le_depth, _LE_OFFS_NULL, le_door->dr_pacc[le_depth] );

                    /* delete class */
                    le_poly_delete( le_class + le_depth );

                }

                /* release buffer memory */
                free( le_buffer );

            }

            /* delete stream */
            fclose( le_stream );

        }

        /* send message */
        return( le_message );

    }

    le_enum_t le_door_io_mono_detect( le_door_t * const le_door, le_address_t const * const le_addr ) {

        /* parsing variable */
        le_size_t le_parse = 0;

        /* size variable */
        le_size_t le_size = le_address_get_size( le_addr );

        /* reset offset */
        le_door->dr_moff = 0;

        /* follow offset */
        do {

            /* read class offset */
            le_door->dr_moff = le_mono_io_offset( le_door->dr_moff, le_address_get_digit( le_addr, le_parse ), * ( le_door->dr_macc + le_parse ) );

            /* check class offset */
            if ( le_door->dr_moff == _LE_OFFS_NULL ) {

                /* send message */
                return( _LE_FALSE );

            }

        /* update index and condition */
        } while ( ( ++ le_parse ) < le_size );

        /* send message */
        return( _LE_TRUE );

    }

    le_enum_t le_door_io_poly_detect( le_door_t * const le_door, le_address_t const * const le_addr ) {

        /* parsing variable */
        le_size_t le_parse = 0;

        /* size variable */
        le_size_t le_size = le_address_get_size( le_addr );

        /* reset offset */
        le_door->dr_poff = 0;

        /* follow offset */
        do {

            /* read class offset */
            le_door->dr_poff = le_poly_io_offset( le_door->dr_poff, le_address_get_digit( le_addr, le_parse ), * ( le_door->dr_pacc + le_parse ) );

            /* check class offset */
            if ( le_door->dr_poff == _LE_OFFS_NULL ) {

                /* send message */
                return( _LE_FALSE );

            }

        /* update index and condition */
        } while ( ( ++ le_parse ) < le_size );

        /* send message */
        return( _LE_TRUE );

    }

    le_void_t le_door_io_mono_gather( le_door_t * const le_door, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array ) {

        /* class variable */
        le_mono_t le_class = LE_MONO_C;

        /* enumeration boundary */
        if ( le_parse == le_span ) {

            /* update array size */
            le_array_set( le_array, LE_ARRAY_DATA );

            /* push element position */
            le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

            /* push element type */
            ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

            /* push element data */
            le_mono_io_data( le_door->dr_moff, le_array_mac_ldata( le_array ), * ( le_door->dr_macc + le_parse ) );

        } else {

            /* read class */
            le_mono_io_read_fast( & le_class, le_door->dr_moff, * ( le_door->dr_macc + le_parse ) );

            /* enumerate daughter classes */
            for ( le_size_t le_digit = 0; le_digit < _LE_USE_BASE; le_digit ++ ) {

                /* extract class offset */
                if ( ( le_door->dr_moff = le_mono_get_offset( & le_class, le_digit ) ) != _LE_OFFS_NULL ) {

                    /* update address digit */
                    le_address_set_digit( le_addr, le_parse, le_digit );

                    /* recursive enumeration */
                    le_door_io_mono_gather( le_door, le_addr, le_parse + 1, le_span, le_array );

                }

            }

        }

    }

    le_void_t le_door_io_poly_gather( le_door_t * const le_door, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array ) {

        /* class variable */
        le_poly_t le_class = LE_POLY_C;

        /* size variable */
        le_size_t le_size = 0;

        /* fallback variable */
        le_size_t le_step = 0;

        /* reading variable */
        le_size_t le_read = 0;

        /* read class - partial */
        le_poly_io_read_fast( & le_class, le_door->dr_poff, * ( le_door->dr_pacc + le_parse ) );

        /* enumeration boundary */
        if ( le_parse == le_span ) {

            /* check class content */
            if ( ( le_size = le_poly_get_size( & le_class ) ) > 0 ) {

                /* read class - completion */
                le_poly_io_read_next( & le_class, * ( le_door->dr_pacc + le_parse ) );

                /* parsing class links */
                for ( le_size_t le_link = 0; le_link < le_size; le_link ++ ) {

                    /* reset read length */
                    le_read = LE_ARRAY_DATA << 1;

                    /* retrieve array size */
                    le_step = le_array_get_size( le_array );

                    /* update array size */
                    le_array_set( le_array, le_read );

                    /* follow link */
                    fseek( le_door->dr_pdat, le_poly_get_link( & le_class, le_link ), SEEK_SET );

                    /* import vertex */
                    if ( fread( le_array_mac_back( le_array, le_read ), sizeof( le_byte_t ), le_read, le_door->dr_pdat ) != le_read ) {

                        /* fallback */
                        le_array_set_size( le_array, le_step );

                    } else {

                        /* check remaining read length */
                        if ( ( le_read = ( * le_array_mac_ltype( le_array ) ) * LE_ARRAY_DATA - le_read ) > 0 ) {

                            /* update array size */
                            le_array_set( le_array, le_read );

                            /* import vertex */
                            if ( fread( le_array_mac_back( le_array, le_read ), sizeof( le_byte_t ), le_read, le_door->dr_pdat ) != le_read ) {

                                /* fallback */
                                le_array_set_size( le_array, le_step );

                            }

                        }

                    }

                }

                /* delete class */
                le_poly_delete( & le_class );

            }

        } else {

            /* enumerate daughter classes */
            for ( le_size_t le_digit = 0; le_digit < _LE_USE_BASE; le_digit ++ ) {

                /* extract class offset */
                if ( ( le_door->dr_poff = le_poly_get_offset( & le_class, le_digit ) ) != _LE_OFFS_NULL ) {

                    /* update address digit */
                    le_address_set_digit( le_addr, le_parse, le_digit );

                    /* recursive enumeration */
                    le_door_io_poly_gather( le_door, le_addr, le_parse + 1, le_span, le_array );

                }

            }

        }

    }

    le_void_t le_door_io_mono_and( le_door_t * const le_pdoor, le_door_t * const le_sdoor, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array ) {

        /* class variable */
        le_mono_t le_pclass = LE_MONO_C;

        /* class variable */
        le_mono_t le_sclass = LE_MONO_C;

        /* secondary state flag */
        le_enum_t le_flag = _LE_TRUE;

        /* enumeration boundary */
        if ( le_parse == le_span ) {

            /* update array size */
            le_array_set( le_array, LE_ARRAY_DATA );

            /* push element position */
            le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

            /* push element type */
            ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

            /* check secondary state */
            if ( le_sdoor->dr_moff != _LE_OFFS_NULL ) {

                /* push element data */
                le_mono_io_data( le_pdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_pdoor->dr_macc + le_parse ) );

            } else {

                /* color modulation */
                ( le_array_mac_ldata( le_array ) )[0] = 32;
                ( le_array_mac_ldata( le_array ) )[1] = 32;
                ( le_array_mac_ldata( le_array ) )[2] = 32;

            }

        } else {

            /* read class offsets */
            if ( le_mono_io_read_fast( & le_pclass, le_pdoor->dr_moff, * ( le_pdoor->dr_macc + le_parse ) ) != LE_ERROR_SUCCESS ) {

                /* fatal error tracking */
                # ifdef _LE_FATAL
                fprintf( stderr, "Fatal : %s at line %d\n", __FILE__, __LINE__ );
                # endif

                /* fatal error */
                return;

            } else {

                /* check secondary state */
                if ( le_sdoor->dr_moff != _LE_OFFS_NULL ) {

                    /* read class offsets */
                    if ( le_mono_io_read_fast( & le_sclass, le_sdoor->dr_moff, * ( le_sdoor->dr_macc + le_parse ) ) != LE_ERROR_SUCCESS ) {

                        /* fatal error tracking */
                        # ifdef _LE_FATAL
                        fprintf( stderr, "Fatal : %s at line %d\n", __FILE__, __LINE__ );
                        # endif

                        /* fatal error */
                        return;

                    } else {

                        /* update secondary state flag */
                        le_flag = _LE_TRUE;

                    }

                } else {

                    /* update secondary state flag */
                    le_flag = _LE_FALSE;

                }

                /* enumerates daughter classes */
                for ( le_size_t le_digit = 0; le_digit < _LE_USE_BASE; le_digit ++ ) {

                    /* check flag */
                    if ( le_flag == _LE_TRUE ) {

                        /* extract offset */
                        le_sdoor->dr_moff = le_mono_get_offset( & le_sclass, le_digit );

                    }
                
                    /* extract and check offset */
                    if ( ( le_pdoor->dr_moff = le_mono_get_offset( & le_pclass, le_digit ) ) != _LE_OFFS_NULL ) {

                        /* update address digit */
                        le_address_set_digit( le_addr, le_parse, le_digit );

                        /* recursive daughter classes enumeration */
                        le_door_io_mono_and( le_pdoor, le_sdoor, le_addr, le_parse + 1, le_span, le_array );

                    }

                }

            }

        }

    }

    le_void_t le_door_io_mono_dd( le_door_t * const le_pdoor, le_door_t * const le_sdoor, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array, le_size_t le_tracker ) {

        /* class variable */
        le_mono_t le_pclass = LE_MONO_C;

        /* class variable */
        le_mono_t le_sclass = LE_MONO_C;

        /* message variable */
        le_enum_t le_pmessage = LE_ERROR_IO_READ;

        /* message variable */
        le_enum_t le_smessage = LE_ERROR_IO_READ;

        /* delayed detection threshold */
        static le_size_t le_threshold[_LE_USE_DEPTH] = {
         1,  1,  1,  1,  1, 
         1,  1,  1,  1,  1,
         1,  1,  1,  1,  1, 
         1,  1,  1,  1,  1,
         2,  2,  3,  3,  4, 
         4,  5,  5,  6,  6,
         7,  7,  8,  8,  9, 
         9, 10, 10, 11, 11
        };

        /* identity detection */
        if ( le_sdoor->dr_moff != _LE_OFFS_NULL ) {

            /* update depth tracker */
            le_tracker = le_parse;

        }

        /* enumeration boundary */
        if ( le_parse == le_span ) {

            /* update array size */
            le_array_set( le_array, LE_ARRAY_DATA );

            /* push element position */
            le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

            /* push element type */
            ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

            /* apply threshold on tracker */
            if ( ( le_span - le_tracker ) >= le_threshold[le_span] ) {

                /* push element data */
                le_mono_io_data( le_pdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_pdoor->dr_macc + le_parse ) );

            } else {

                /* color modulation */
                ( le_array_mac_ldata( le_array ) )[0] = 32;
                ( le_array_mac_ldata( le_array ) )[1] = 32;
                ( le_array_mac_ldata( le_array ) )[2] = 32;

            }

        } else {

            /* check offset */
            if ( le_pdoor->dr_moff != _LE_OFFS_NULL ) {

                /* read class */
                le_pmessage = le_mono_io_read_fast( & le_pclass, le_pdoor->dr_moff, * ( le_pdoor->dr_macc + le_parse ) );

            }

            /* check offset */
            if ( le_sdoor->dr_moff != _LE_OFFS_NULL ) {

                /* read class */
                le_smessage = le_mono_io_read_fast( & le_sclass, le_sdoor->dr_moff, * ( le_sdoor->dr_macc + le_parse ) );

            }

            /* enumerate daughter classes */
            for ( le_size_t le_digit = 0; le_digit < _LE_USE_BASE; le_digit ++ ) {

                /* check message */
                if ( le_pmessage == LE_ERROR_SUCCESS ) { /* not needed */

                    /* extract class offset */
                    le_pdoor->dr_moff = le_mono_get_offset( & le_pclass, le_digit );

                /* reset offset */
                } else { le_pdoor->dr_moff = _LE_OFFS_NULL; } /* not needed */

                /* check message */
                if ( le_smessage == LE_ERROR_SUCCESS ) {

                    /* extract class offset */
                    le_sdoor->dr_moff = le_mono_get_offset( & le_sclass, le_digit );

                /* reset offset */
                } else { le_sdoor->dr_moff = _LE_OFFS_NULL; }

                /* check offsets */
                if ( le_pdoor->dr_moff != _LE_OFFS_NULL ) {

                    /* update address digit */
                    le_address_set_digit( le_addr, le_parse, le_digit );

                    /* recursive enumeration */
                    le_door_io_mono_dd( le_pdoor, le_sdoor, le_addr, le_parse + 1, le_span, le_array, le_tracker );

                }

            }

        }

    }

    le_void_t le_door_io_mono_parallel( le_door_t * const le_pdoor, le_door_t * const le_sdoor, le_address_t * const le_addr, le_byte_t const le_mode, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array ) {

        /* class variable */
        le_mono_t le_pclass = LE_MONO_C;

        /* class variable */
        le_mono_t le_sclass = LE_MONO_C;

        /* message variable */
        le_enum_t le_pmessage = LE_ERROR_IO_READ;

        /* message variable */
        le_enum_t le_smessage = LE_ERROR_IO_READ;

        /* enumeration boundary */
        if ( le_parse == le_span ) {

            /* switch on mode */
            if ( le_mode == LE_ADDRESS_OR ) {

                /* update array size */
                le_array_set( le_array, LE_ARRAY_DATA );

                /* push element position */
                le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

                /* push element type */
                ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

                /* logical or operator */
                if ( le_pdoor->dr_moff != _LE_OFFS_NULL ) {

                    /* push element data */
                    le_mono_io_data( le_pdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_pdoor->dr_macc + le_parse ) );

                } else {

                    /* push element data */
                    le_mono_io_data( le_sdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_sdoor->dr_macc + le_parse ) );

                }

            } else if ( le_mode == LE_ADDRESS_AND ) {

                /* logical and operator */
                if ( ( le_pdoor->dr_moff != _LE_OFFS_NULL ) && ( le_sdoor->dr_moff != _LE_OFFS_NULL ) ) {

                    /* update array size */
                    le_array_set( le_array, LE_ARRAY_DATA );

                    /* push element position */
                    le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

                    /* push element type */
                    ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

                    /* push element data */
                    le_mono_io_data( le_pdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_pdoor->dr_macc + le_parse ) );

                }

            } else if ( le_mode == LE_ADDRESS_XOR ) {

                /* logical xor operator */
                if ( ( le_pdoor->dr_moff != _LE_OFFS_NULL ) && ( le_sdoor->dr_moff == _LE_OFFS_NULL ) ) {

                    /* update array size */
                    le_array_set( le_array, LE_ARRAY_DATA );

                    /* push element position */
                    le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

                    /* push element type */
                    ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

                    /* push element data */
                    le_mono_io_data( le_pdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_pdoor->dr_macc + le_parse ) );

                } else if ( ( le_pdoor->dr_moff == _LE_OFFS_NULL ) && ( le_sdoor->dr_moff != _LE_OFFS_NULL ) ) {

                    /* update array size */
                    le_array_set( le_array, LE_ARRAY_DATA );

                    /* push element position */
                    le_address_get_pose_( le_addr, le_parse, le_array_mac_lpose( le_array ) );

                    /* push element type */
                    ( * le_array_mac_ltype( le_array ) ) = LE_UV3_POINT;

                    /* push element data */
                    le_mono_io_data( le_sdoor->dr_moff, le_array_mac_ldata( le_array ), * ( le_sdoor->dr_macc + le_parse ) );

                }

            }

        } else {

            /* check offset */
            if ( le_pdoor->dr_moff != _LE_OFFS_NULL ) {

                /* read class */
                le_pmessage = le_mono_io_read_fast( & le_pclass, le_pdoor->dr_moff, * ( le_pdoor->dr_macc + le_parse ) );

            }

            /* check offset */
            if ( le_sdoor->dr_moff != _LE_OFFS_NULL ) {

                /* read class */
                le_smessage = le_mono_io_read_fast( & le_sclass, le_sdoor->dr_moff, * ( le_sdoor->dr_macc + le_parse ) );

            }

            /* enumerate daughter classes */
            for ( le_size_t le_digit = 0; le_digit < _LE_USE_BASE; le_digit ++ ) {

                /* check message */
                if ( le_pmessage == LE_ERROR_SUCCESS ) {

                    /* extract class offset */
                    le_pdoor->dr_moff = le_mono_get_offset( & le_pclass, le_digit );

                /* reset offset */
                } else { le_pdoor->dr_moff = _LE_OFFS_NULL; }

                /* check message */
                if ( le_smessage == LE_ERROR_SUCCESS ) {

                    /* extract class offset */
                    le_sdoor->dr_moff = le_mono_get_offset( & le_sclass, le_digit );

                /* reset offset */
                } else { le_sdoor->dr_moff = _LE_OFFS_NULL; }

                /* check offsets */
                if ( ( le_pdoor->dr_moff != _LE_OFFS_NULL ) || ( le_sdoor->dr_moff != _LE_OFFS_NULL ) ) {

                    /* update address digit */
                    le_address_set_digit( le_addr, le_parse, le_digit );

                    /* recursive enumeration */
                    le_door_io_mono_parallel( le_pdoor, le_sdoor, le_addr, le_mode, le_parse + 1, le_span, le_array );

                }

            }

        }

    }

