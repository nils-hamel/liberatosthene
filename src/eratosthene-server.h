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

    /*! \file   eratosthene-server.h
     *  \author Nils Hamel <nils.hamel@alumni.epfl.ch>
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
    # include "eratosthene-door.h"
    # include "eratosthene-switch.h"
    # include "eratosthene-uv3.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_SERVER_C { _LE_SOCK_NULL, _LE_SOCK_NULL, NULL, 0, 0, LE_MUTE_C, LE_ERROR_SUCCESS }

/*
    header - preprocessor macros
 */

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
     *  access and network broadcasting of colorimetric points and polygonal
     *  representations. Its storage structure, data injection and data query
     *  are driven by the formalism of spatiotemporal index. In other words, the
     *  server can be seen as a 4D tile server based on the theoretical
     *  framework of spatial index.
     *
     *  For the point based models, the spatial index are defining equivalence
     *  classes in which points are collected to create a natural multi-scale
     *  representation of the 4D earth. For polygonal models, these classes are
     *  used as container of links toward the relevant graphical primitives
     *  relevant to display at a given scale.
     *
     *  A server is then responsible of receiving data from client during
     *  injection, data broadcasting during client query, broadcasting of its
     *  main parameter through client specific query and the storage management
     *  of the data. This structure is then the heart allowing these operations
     *  to be fulfilled, with the help of the library sub-systems.
     *
     *  A first field is dedicated to store the server socket descriptor. As any
     *  server, an eratosthene server maintains a service on a given computer
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
     *  The last field is used during client connection management. As a client
     *  connects to the server, the socket descriptor is pushed using this field
     *  before to broadcast it to the client thread. The server is then creating
     *  a specific thread for each client connection. This thread is responsible
     *  of answering any type of query the client performs until the client
     *  disconnects.
     *
     *  As the server offers a 4D information system through the formalism of
     *  spatiotemporal index, one has to take into account geographic ranges
     *  allowed for indexation. For both longitude and latitude, the canonical
     *  ranges are considered (with coordinates always in radian) : [-pi,pi] and
     *  [-pi/2,pi/2]. A more specific range is considered for height values
     *  (with coordinates always in meter above WGS84 ellipsoid) : [-h,h] where
     *  h equal to \b LE_ADDRESS_MAXH (address module). Any element, in terms of
     *  its position coordinates, injected in the server has to be inside the
     *  defined ranges.
     *
     *  As mentioned, for point-based model, storage and access are driven by
     *  the spatio-temporal index. Any point-based element is assumed to be
     *  referenced in space and time according to the previous frame and ranges.
     *  Two groups of equivalence relations are then defined, one on the time
     *  range and the others on the spatial scales. The time relation simply
     *  establish homogeneous ranges along the time dimension collapsing all
     *  incoming data in within these range, making them indistinguishable from
     *  this point of view. The temporal parameter of the server then gives the
     *  size, in seconds of these temporal ranges.
     *
     *  For polygonal models, the same logic applies from the temporal point of
     *  view, collapsing all primitives within a time range in a single storage
     *  structure, making them linked from this point of view.
     *
     *  For point-based models, spatial scales are used to defined equivalence
     *  relation grouping the incoming points. The number of scales is given by
     *  the server spatial configuration parameter. As the index of the scale
     *  increase, the more equivalence classes are defined, increasing the power
     *  of resolution between the injected data. This allows to query low scale
     *  to get a general view of the earth structure while querying higher scale
     *  allows to see smaller structures with a much better resolution. On each
     *  scale, i being the index of the scale, 2 ^ i equivalences classes are
     *  defined homogeneously along each spatial dimension. The points position
     *  coordinates are used to determine in which class they belong for all the
     *  defined scale.
     *
     *  It follows, for point based models, that querying data is only a
     *  question of following the index digits to find the equivalence classes
     *  of the desired data. In other words, due to the storage structure, no
     *  search is performed, i.e. no comparison, to access the data. Gathering
     *  the desired data is only a question of an enumeration.
     *
     *  For polygonal models, the same equivalence relation are defined on the
     *  space scale in order to allow the same efficiency in data access. For
     *  such models, more complicated than simple points, the class are seen as
     *  container linking to the actual graphical primitives that defines the
     *  model to broadcast to the clients. The injection logic is to assign a
     *  graphical primitive to a specific class according to its first vertex
     *  position and to a specific scale looking at the size of the primitive.
     *  This allows to make primitive appearing during query according to their
     *  display relevance. Small primitives, showing a city detail for example,
     *  has no to be considered in case of a query for a general view of the
     *  earth.
     *
     *  The server then allows to consider any type of model, regardless of
     *  their weight, complexity or the scale at which they describe structures
     *  of the earth. The server allows to browse all these data in both space
     *  and time to offer a detailed and 4D representation of the earth and
     *  allowing to compare any kind of geographical data.
     *
     *  \var le_server_struct::sv_sock
     *  Server socket descriptor
     *  \var le_server_struct::sv_push
     *  Client socket descriptor exchanger
     *  \var le_server_struct::sv_path
     *  Server storage structure path
     *  \var le_server_struct::sv_scfg
     *  Server spatial configuration parameter (scale count)
     *  \var le_server_struct::sv_tcfg
     *  Server temporal configuration parameter
     *  \var le_server_struct::sv_mute
     *  Server/client mutual exclusion descriptor
     *  \var le_server_struct::_status
     *  Standard status field
     */

    typedef struct le_server_struct {

        le_sock_t   sv_sock;
        le_sock_t   sv_push;

        le_char_t * sv_path;
        le_size_t   sv_scfg;
        le_time_t   sv_tcfg;

        le_mute_t   sv_mute;

    le_enum_t _status; } le_server_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function creates a server structure and initialises its content
     *  according to the provided parameters. It reads the server configuration
     *  file hold in the storage structure pointed by the provided path.
     *
     *  It also creates the socket descriptor used by the server to listen to
     *  client connections.
     *
     *  This function returning the created structure, the status is stored in
     *  the structure itself using the reserved \b _status field.
     *
     *  \param le_path Server storage structure path
     *  \param le_port Server service port
     *
     *  \return Returns created server structure
     */

    le_server_t le_server_create( le_char_t * const le_path, le_sock_t const le_port );

    /*! \brief constructor/destructor methods
     *
     *  This function deletes the server structure provided as parameter by
     *  closing the listening socket descriptor. The function ends by clearing
     *  the fields of the structure using default values.
     *
     *  \param le_server Server structure
     */

    le_void_t le_server_delete( le_server_t * const le_server );

    /*! \brief mutator methods
     *
     *  This function reads the configuration file at the root of the server
     *  storage structure. To locate the storage structure, this function uses
     *  the path hold by the structure itself, assumed to be correctly filled.
     *
     *  The function reads the two space and time configuration values stored in
     *  the file (text mode) and performs basic consistency checks.
     *
     *  \param le_server Server structure
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_server_set_config( le_server_t * const le_server );

    /*! \brief i/o methods
     *
     *  With the server structure creation and deletion methods, this function
     *  is part of the server main element. As a created structure is provided,
     *  the function maintains the server service and addresses clients remote
     *  connections.
     *
     *  This function is executed by the main thread and is responsible of the
     *  management of all incoming client connections. As a client is accepted,
     *  the function creates a thread for the management of its connection. The
     *  thread is created using pthread library.
     *
     *  The function pushes the newly connected clients socket descriptor in the
     *  server structure reserved field before to create the client thread. The
     *  function waits, using a mutual exclusion locker, that the thread takes
     *  its client socket descriptor. The mutual exclusion is then unlock to
     *  allow the function to accept another client.
     *
     *  \param le_server Server structure
     */

    le_void_t le_server_io( le_server_t * const le_server );

    /*! \brief i/o methods (outdated documentation)
     *
     *  This function is the main client connection management procedure. It is
     *  usually called as a pthread function after the acceptation of a new
     *  client connection by the main thread.
     *
     *  In the first place, the thread uses a mutual exclusion to ensure it got
     *  the correct client socket descriptor during the reading of the socket
     *  push field of the provided server structure.
     *
     *  The function implements the server services through a loop waiting for
     *  client queries. The communication between the client and the server is
     *  made through socket-arrays that packs the communicated information. The
     *  loop implements a switch used to determine which query is made by the
     *  client to answer it accordingly. Specialised functions are used through
     *  this switch to properly answer client requests.
     *
     *  As the client disconnect or if any critical error occurs, the function
     *  closes the client connection and exits returning a NULL pointer.
     *
     *  \param le_void Pointer to a server structure (casted as void * )
     *
     *  \return Returns always a NULL pointer.
     */

    le_void_t * le_server_io_client( le_void_t * le_void );

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

