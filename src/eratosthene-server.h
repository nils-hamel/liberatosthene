/*
 *  liberatosthene
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016-2018 DHLAB, EPFL
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

    /*! \file   eratosthene-server.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - server
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_SERVER__
    # define __LE_SERVER__

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    extern "C" {
    # endif

/*
    header - internal includes
 */

    # include "eratosthene.h"
    # include "eratosthene-address.h"
    # include "eratosthene-array.h"
    # include "eratosthene-client.h"
    # include "eratosthene-tree.h"
    # include "eratosthene-unit.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_SERVER_C                { _LE_SOCK_NULL, NULL, 0, 0, { LE_SERVER_ACTIVE, 0 }, LE_ERROR_SUCCESS }

    /* define pool messsage */
    # define LE_SERVER_ACTIVE           ( 0x01 )
    # define LE_SERVER_RELOAD           ( 0x02 )

/*
    header - preprocessor macros
 */

    /* define access marco */
    # define le_server_mac_pool(s,i,m)  ( ( s )->sv_pool[i] & ( m ) )

    /* define access marco */
    # define le_server_mac_set(s,i,m)   ( ( s )->sv_pool[i] |= ( m ) )

    /* define access marco */
    # define le_server_mac_clear(s,i,m) ( ( s )->sv_pool[i] &= ( ~ ( m ) ) )

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_server_struct
     *  \brief server structure
     *
     *  This structure is the principal structure of the library as it holds the
     *  server main elements.
     *
     *  The server offers a 4D mapping service of the earth through storage,
     *  access and network broadcasting of colorimetric points. Its storage
     *  structure, data injection and data query are driven by the formalism
     *  of spatiotemporal index. In other words, the server can be seen as a
     *  4D tile server based on the theoretical framework of index.
     *
     *  A server is then responsible of receiving data from client during
     *  injection, data broadcasting during client query, broadcasting of its
     *  main parameter through client specific query and the storage management
     *  of the data. This structure is then the heart allowing these operations
     *  to be fulfilled.
     *
     *  A first field is dedicated to store the server socket descriptor. As any
     *  server, an eratosthene server maintain a service on a computer available
     *  through a predefined port. This field store the socket descriptor set
     *  to listen to client connections.
     *
     *  Three fields are dedicated to the server configuration. The first one
     *  holds a characters array storing the main path of the storage structure.
     *  This path is used to access storage structure files. The two remaining
     *  configuration fields are used to store the space and time parameters,
     *  The space parameter gives the number of scale engaged in the storage
     *  structure. It follows that any space index used to access or store data
     *  has to contain less digit than this configuration value. The time value
     *  gives the size of the temporal equivalence classes. Both parameters fix
     *  the server resolution power in both space and time.
     *
     *  The next field, an array of the size corresponding to the amount of
     *  thread used by the server to handle client connection, is used for
     *  the communication between the threads. Its elements are used as bit
     *  container to broadcast specific messages. For example, as a new time
     *  unit storage has been created by a thread, a message is sent through
     *  this array to tell the other threads to take into account this new
     *  data.
     *
     *  A last field is used by the structure creation methods to specified the
     *  creation status of the structure. This allows to creation methods to
     *  return the structure instead of an error code.
     *
     *  As the server offers a 4D information system through the formalism of
     *  spatiotemporal index, one has to take into account geographic ranges
     *  allowed by indexation. For both longitude and latitude, the canonical
     *  ranges are considered (with coordinates always in radian) : [-pi,pi] and
     *  [-pi/2,pi/2]. A more specific range is considered for height values
     *  (with coordinates always in meter above WGS84 ellipsoid) : [-h,h] where
     *  h equal to \b LE_ADDRESS_MAXH (address module). Any point injected in
     *  the server has to be inside the defined ranges.
     *
     *  The server data storage and access are driven by the spatiotemporal
     *  index formalism. As any point is assumed to be referenced in space and
     *  time, two equivalence relations are defined : one on the time range and
     *  the other along the spatial scales. The time relation simply establish
     *  homogeneous ranges along the time dimension collapsing all time in each
     *  range to a single equivalence class. It follow that two time falling in
     *  the same class cannot be distinguished anymore after injection. The
     *  structure time parameter gives the size, in seconds, of time equivalence
     *  classes.
     *
     *  Each temporal class holds a spatial storage unit. Each unit contain
     *  scales (files) in amount given by the structure spatial configuration
     *  value. On the model of the relation defined on time, a similar relation
     *  is set on each scale : each scale, in term of range, is split in 2^i
     *  ranges identified as equivalence classes, i being the scale number. Any
     *  given point then belong to a given class at each scale. Index are used,
     *  through their digits, to browse the spatial scales to find or store any
     *  point. Again, two points falling in the same class in a specific scale
     *  cannot be separated after injection. In other words, index digits are
     *  the address allowing to go from a scale to another following the logic
     *  of octrees.
     *
     *  \var le_server_struct::sv_sock
     *  Server socket descriptor
     *  \var le_server_struct::sv_root
     *  Server storage structure path
     *  \var le_server_struct::sv_scfg
     *  Server spatial configuration value : number of scale
     *  \var le_server_struct::sv_tcfg
     *  Server temporal configuration value : size of temporal classes
     *  \var le_server_struct::sv_pool
     *  Server thread pooling message
     *  \var le_server_struct::_status
     *  Standard status field
     */

    typedef struct le_server_struct {

        le_sock_t   sv_sock;

        le_char_t * sv_root;
        le_size_t   sv_scfg;
        le_time_t   sv_tcfg;

        le_byte_t   sv_pool[_LE_USE_PENDING];

    le_enum_t _status; } le_server_t;

    /*! \struct le_pack_struct
     *  \brief server structure
     *
     *  This structure is used to broadcast the required parameters for a thread
     *  to manage an accepted client connection. The main thread is responsible
     *  of accepting the client incoming connection before to create a thread
     *  associated to each client.
     *
     *  The main thread then packs the required information in this structure
     *  that is passed to the created thread.
     *
     *  \var le_pack_struct::p_s
     *  Server structure pointer
     *  \var le_pack_struct::p_t
     *  Thread process ID
     *  \var le_pack_struct::p_c
     *  Thread client socket descriptor
     *  \var le_pack_struct::p_m
     *  Mutual exclusion for thread synchronisation
     */

    typedef struct le_pack_struct {

        le_server_t     * p_s;
        le_size_t         p_t;
        le_sock_t         p_c;
        pthread_mutex_t   p_m;

    } * _pck, le_pack_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function creates a server structure and initialises its content
     *  according to the provided parameters. It reads the server configuration
     *  file hold in the storage structure pointed by the provided path.
     *
     *  It also create the socket descriptor used by the server to listen to
     *  client connections.
     *
     *  This function returning the created structure, the status is stored in
     *  the structure itself using the reserved \b _status field.
     *
     *  \param le_root Server storage structure path
     *  \param le_port Server service port
     *
     *  \return Returns created server structure
     */

    le_server_t le_server_create( le_char_t * const le_root, le_sock_t const le_port );

    /*! \brief constructor/destructor methods
     *
     *  This function deletes the server structure provided as parameter by
     *  closing the socket descriptor. The function ends by clearing the fields
     *  of the structure.
     *
     *  \param le_server Server structure
     */

    le_void_t le_server_delete( le_server_t * const le_server );

    /*! \brief accessor methods
     *
     *  This function searches an available thread usually for the management of
     *  a client connection.
     *
     *  The function searches in the thread pooling array to determine which
     *  thread is available. If a thread is found, its index is returned by the
     *  function.
     *
     *  If all thread are occupied, the function returns the \b _LE_USE_PENDING
     *  value that describe the size of the thread stack.
     *
     *  \param le_server Server structure
     *
     *  \return Returns available thread ID, _LE_USE_PENDING otherwise
     */

    le_size_t le_server_get_thread( le_server_t * const le_server );

    /*! \brief mutator methods
     *
     *  This function reads the configuration file at the root of the server
     *  storage structure. To locate the storage structure, this function uses
     *  the path hold by the structure itself, assumed to be correctly filled.
     *
     *  The function opens and reads the two space and time configuration values
     *  stored in the file (text mode) and perform basic consistency checks.
     *
     *  \param le_server Server structure
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_server_set_config( le_server_t * const le_server );

    /*! \brief mutator methods
     *
     *  This function allows to broadcast a modification of the state of the
     *  thread pooling element to all thread except the provided one.
     *
     *  \param le_server  Server structure
     *  \param le_tid     Thread ID
     *  \param le_message Message mask
     */

    le_void_t le_server_set_broadcast( le_server_t * const le_server, le_size_t const le_tid, le_byte_t const le_message );

    /*! \brief mutator methods
     *
     *  This function is used to check if the specified thread need to reload
     *  its tree structure by looking if such a pooling message have been
     *  broadcasted.
     *
     *  In such a case, the function deletes the provided tree structure before
     *  to re-create it, taking into account the changes responsible of the
     *  message broadcast.
     *
     *  \param le_server  Server structure
     *  \param le_tid     Thread ID
     *  \param le_tree    Tree structure
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_server_set_tree( le_server_t * const le_server, le_size_t const le_tid, le_tree_t * const le_tree );

    /*! \brief i/o methods
     *
     *  With the server structure creation and deletion methods, this function
     *  is part of the server main element. As a created structure is provided,
     *  the function maintains the server service and addresses clients queries.
     *
     *  This function is executed by the main thread and is responsible of the
     *  management of all incoming client connections. As a client is accepted,
     *  the function creates a thread for the management of its connection. The
     *  thread is created using pthread library.
     *
     *  The function packs the required arguments and parameters in a specific
     *  packing structure before to send it the the created thread. It then
     *  ensures the reception of the arguments and parameters by the thread
     *  before listening again for other client connections.
     *
     *  \param le_server Server structure
     */

    le_void_t le_server_io( le_server_t * const le_server );

    /*! \brief i/o methods
     * 
     *  This function is the main client connection management procedure. It is
     *  usually called as a pthread function after the acceptation of a new
     *  client connection by the main thread.
     *
     *  The function implements the server services through a loop waiting for
     *  client queries. The communication between the client and the server is
     *  made through socket-arrays that packs the communication information. The
     *  loop implements a switch used to determine which query is made by the
     *  client to answer it accordingly. Specialised functions are used through
     *  this switch to properly answer client requests.
     *
     *  As the client disconnect or if an critical error occurs, the function
     *  closes the client connection and exits returning a NULL pointer.
     *
     *  \param le_pack pthread arguments and parameters packing structure
     *
     *  \return Always a NULL pointer
     */

    le_void_t * le_server_io_client( le_void_t * le_pack );

    /*! \brief i/o methods
     *
     *  This i/o method is responsible of answering server configuration request
     *  from client. It simply packs the server configuration values, that are
     *  the number of space scale and the time equivalences classes length, in
     *  an array structure before to send it back to the remote client.
     *
     *  \param le_server Server structure
     *  \param le_tree   Tree structure
     *  \param le_stack  Socket-array stack
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_server_io_auth( le_server_t * const le_server, le_tree_t * const le_tree, le_array_t * const le_stack, le_sock_t const le_socket );

    /*! \brief i/o methods
     *
     *  This i/o methods is responsible of data injection in the server storage
     *  structure. It expects a time packed in the first client array that is
     *  used to access the storage structure. It then expects a data array that
     *  contains the geographic coordinates and colour of the points to inject
     *  in the server storage structure.
     *
     *  \param le_server Server structure
     *  \param le_tree   Tree structure
     *  \param le_stack  Socket-array stack
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_server_io_inject( le_server_t * const le_server, le_tree_t * const le_tree, le_array_t * const le_stack, le_sock_t const le_socket );

    /*! \brief i/o methods
     *
     *  This function allows client to query a specific time storage allocation
     *  optimisation. The function reads the time packed in the socket-array and
     *  search for the corresponding storage unit. It then runs the optimisation
     *  process on the found unit.
     *
     *  \param le_server Server structure
     *  \param le_tree   Tree structure
     *  \param le_stack  Socket-array stack
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_server_io_optm( le_server_t * const le_server, le_tree_t * const le_tree, le_array_t * const le_stack, le_sock_t const le_socket );

    /*! \brief i/o methods
     *
     *  This function reads the query addresses packed in the client array and
     *  gather the relevant data through specific tree method.
     *
     *  As the points are gathered from the storage structure, the function
     *  writes the resulting data array on the client socket.
     *
     *  \param le_server Server structure
     *  \param le_tree   Tree structure
     *  \param le_stack  Socket-array stack
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_server_io_query( le_server_t * const le_server, le_tree_t * const le_tree, le_array_t * const le_stack, le_sock_t const le_socket );

/*
    header - C/C++ compatibility
 */

    # ifdef __cplusplus
    }
    # endif

/*
    header - inclusion guard
 */

    # endif

