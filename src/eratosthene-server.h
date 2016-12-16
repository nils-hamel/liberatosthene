/*
 *  liberatosthene - eratosthene indexation server
 *
 *      Nils Hamel - nils.hamel@bluewin.ch
 *      Copyright (c) 2016 EPFL CDH DHLAB
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
     *  \author Nils Hamel <n.hamel@bluewin.ch>
     *
     *  Server module
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
    # include "eratosthene-class.h"
    # include "eratosthene-client.h"
    # include "eratosthene-stream.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_SERVER_C { _LE_SOCK_NULL, NULL, 0, 0, 1, LE_STREAM_C, LE_ERROR_SUCCESS }

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
     *  This structure is the principal structure of the library as it holds
     *  the description of the server.
     *
     *  The "erathosthene" system is designed to offers a worldwide 4D tile
     *  server of informations coming with a three-dimensional georeference and
     *  a time coordinates. Using hierarchical equivalence classes to drive its
     *  storage structure, it allows high-performance queries on the refereneced
     *  data it stores. Currently, the eratosthene system is developed to manage
     *  colorimetric points as referenced data to provide an historical geographic
     *  information system of the 3D history of earth.
     *
     *  As every server, the eratosthene server expects client requests about
     *  4D informations. According to the request time and spatial definition,
     *  it searches the data into its storage structure and send them back to
     *  the clients.
     *
     *  It also allow client to inject 4D information in the storage structure
     *  using internet protocol. The server reads the client 4D information and
     *  injects them in the storage structure. In addition, it is also capable
     *  of answering query about available times and about its configuration.
     *
     *  The eratosthene server is built around its data storage strategy and
     *  management. The geographics coordinates are considered and ranges are
     *  defined : [0,2pi[ for longitude, [-pi/2,pi/2] for the latitude and
     *  [hmin,hmax] for the ellipsoid heights (WGS84 is considered as the
     *  server reference ellipsoid). Considering scales, from zero to a highest
     *  scale (\b sv_scfg parameter), it devides each dimension ranges into
     *  2^k homogeneous equivalence classes, where k gives the considered scale.
     *  As a point is injected in the storage structure, it is added to each
     *  classes along the scales it belong to. This octree structure allows to
     *  perform high-performances queries on spatial structure.
     *
     *  As any information stored in the server storage structure, the points
     *  are associated with a time value that drives their storage. The value
     *  \b sv_tcfg gives the size of the uni-scale temporal equivalence classes
     *  defined on time. It is used to determine in which time allocation the
     *  data has to be stored. In other words, each time equivalence classes
     *  has its own spatial storage structure as decribed in the last paragraph.
     *
     *  The \b sv_path field simply stores the root path of the server storage
     *  structure in which is expected the configuration file containing the
     *  server configuration paramters (\b sv_scfg and \b sv_tcfg, simply
     *  stored in a text file).
     *
     *  As each active temporal classes comes with its own spatial storage
     *  structure, this structure holds a simple file streams management
     *  strategy. It stores a stack of file streams (\b sv_file) that contains
     *  the \b sv_tcfg required file stream to access the scales. The temporal
     *  description of each opened file streams is stored using the \b sv_time
     *  array, which is always of the same size of the file streams stack.
     *  The \b sv_push value simply indicates which stack allocation to use if
     *  the required file streams if not already opened in the file streams
     *  stack.
     *
     *  \var le_server_struct::sv_sock
     *  Server socket descriptor
     *  \var le_server_struct::sv_path
     *  Server storage structure local path
     *  \var le_server_struct::sv_scfg
     *  Server number of spatial scales in the storage structure
     *  \var le_server_struct::sv_tcfg
     *  Server time discretisation parameter in the storage structure
     *  \var le_server_struct::sv_push
     *  Number of active streams in the stream stack
     *  \var le_server_struct::sv_time
     *  Stack of Times of the active streams
     *  \var le_server_struct::sv_file
     *  Stack of files descriptors of the active streams
     */

    typedef struct le_server_struct {

        le_sock_t   sv_sock;

        le_char_t * sv_path;
        le_size_t   sv_scfg;
        le_time_t   sv_tcfg;
        le_size_t   sv_ncfg;

        le_stream_t sv_stream;

    le_enum_t _status; } le_server_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function creates a server structure and initialises it contents
     *  according to the provided parameters. It initialise the streams stack
     *  and reads the server configuration file hold in the storage structure.
     *
     *  It also create the socket descriptor used by the server to listen to
     *  client queries.
     *
     *  \param le_port Server service port
     *  \param le_path Server storage structure local path
     *
     *  \return Returns created server structure
     */

    le_server_t le_server_create( le_char_t * const le_path, le_sock_t const le_port );

    /*! \brief constructor/destructor methods
     *
     *  This function deletes the server structure provided as parameter by
     *  closing the socket descriptor and by closing every openned file
     *  descriptor present in the streams stack.
     *
     *  \param le_server Server structure
     */

    le_void_t le_server_delete( le_server_t * const le_server );

    /*! \brief server methods
     *
     *  This function is the server main function. It holds the server loop
     *  listening for incomming client queries. As a query is revieved, this
     *  function handle the server-side handshake procedure and answer the
     *  client by calling the specific server sub-process.
     *
     *  \param le_server Server structure
     */

    le_void_t le_server( le_server_t * const le_server );

    /*! \brief client methods - injection
     *
     *  This function is a specific server sub-process.
     *
     *  Its role is to provides points injection service. It expects the client
     *  to send colorimetric points stream (RF format). As points are streamed,
     *  this function sends them to its specific sub-process responsible of the
     *  insertion of the incoming points in the storage structure.
     *
     *  \param le_server Server structure
     *  \param le_client Server-side client socket descriptor
     */

    le_void_t le_server_inject_client( le_server_t * const le_server, le_sock_t const le_client );

    le_void_t le_server_reduce_client( le_server_t * const le_server, le_sock_t const le_client );

    /*! \brief client methods - query
     *
     *  This function is a specific server sub-process.
     *
     *  Its role is to answer to points queries. It expects the client to send
     *  an address structure that points the equvalence class (or its daughters)
     *  containing the points requested by the client. It then gathers the
     *  points in a simple array structure and sends it back to the client.
     *
     *  \param le_server Server structure
     *  \param le_client Server-side client socket descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_void_t le_server_query_client( le_server_t * const le_server, le_sock_t const le_client );

    /*! \brief client methods - server configuration
     *
     *  This function is a specific server sub-process.
     *
     *  Its role is to allow clients to retrieve the server configuration values
     *  use to drive and manage the storage structure.
     *
     *  \param le_server Server structure
     *  \param le_client Server-side client socket descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_void_t le_server_config_client( le_server_t const * const le_server, le_sock_t const le_client );

    le_enum_t le_server_set_config( le_server_t * const le_server );

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

