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

    /*! \file   eratosthene-switch.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - switch
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_SWITCH__
    # define __LE_SWITCH__

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
    # include "eratosthene-door.h"
    # include "eratosthene-operator.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_SWITCH_C            { 0, NULL, 0, 0, NULL, LE_ERROR_SUCCESS }

    /* define pseudo-constructor */
    # define LE_SWITCH_C_PST(p,s,t) { 0, p, s, t, NULL, LE_ERROR_SUCCESS }

/*
    header - preprocessor macros
 */

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_switch_struct
     *  \brief tree structure
     *
     *  This structure holds the backbone elements to access a server storage
     *  structure. It maintains a linked list of door structures that are used
     *  for specialised data access through the door module functions.
     *
     *  The structure is mainly responsible of maintaining the door structures
     *  linked list. This linked list is the first element to access for data
     *  query for a remote client. It is used as a search backbone to detect
     *  in which temporal storage structure to search the relevant data.
     *
     *  The maintained linked list provided a sorted, from the temporal point of
     *  view, of the active doors of a specific server implementation. Each door
     *  is responsible for the management of the data access on its dedicated
     *  temporal storage structure.
     *
     *  This module is also used as a front-end to client queries. The module
     *  function searches the relevant door structure before to broadcast the
     *  access request to the relevant door engaged in the linked list. From
     *  this point of view, this module is used by the functions of the server
     *  module to process the client queries.
     *
     *  The first field of the structure is used to store the last linked list
     *  update clock time. This allows the structure to know for how many time
     *  it has been kept un-updated. This allows to trigger the linked list
     *  update each given amount time. This procedure is set to simplify the
     *  communication between the thread to keep them synchronised, in terms of
     *  their view of the storage structure, when a thread inject new data.
     *  Doing so allows to avoid to setup a complicated communication system
     *  between the thread to keep track of newly injected data.
     *
     *  The three next field are a simple copy of the server configuration data
     *  for a given implementation. These data includes storage structure path,
     *  spatial and temporal configuration values.
     *
     *  The last field is a pointer to the first door structure engaged in the
     *  linked list hold by this switch structure. It is used to search along
     *  the active door to find the relevant one during client queries.
     *
     *  A last field is used by the structure creation methods to specified the
     *  creation status of the structure. This allows to creation methods to
     *  return the structure instead of an error code.
     *
     *  \var le_switch_struct::sw_load
     *  Clock time of the last linked list update
     *  \var le_switch_struct::sw_path
     *  Server storage structure path - copy
     *  \var le_switch_struct::sw_scfg
     *  Server spatial configuration parameter - copy
     *  \var le_switch_struct::sw_tcfg
     *  Server temporal configuration parameter - copy
     *  \var le_switch_struct::sw_door
     *  Doors linked list first (lower in time) pointer
     *  \var le_switch_struct::_status
     *  Standard status field
     */

    typedef struct le_switch_struct {

        le_time_t   sw_load;

        le_char_t * sw_path;
        le_size_t   sw_scfg;
        le_time_t   sw_tcfg;

        le_door_t * sw_door;

    le_enum_t _status; } le_switch_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor method
     *
     *  This function creates a switch structure and returns it. It simply
     *  initialises the fields of the structure with default values and assigns
     *  the provided parameters to their respective fields.
     *
     *  This function returning the created structure, the status is stored in
     *  the structure itself using the reserved \b _status field.
     *
     *  \param le_path Server storage structure path
     *  \param le_scfg Server spatial configuration value
     *  \param le_tcfg Server temporal configuration value
     *
     *  \return Returns the created structure
     */

    le_switch_t le_switch_create( le_char_t * const le_path, le_size_t const le_scfg, le_time_t const le_tcfg );

    /*! \brief constructor/destructor methods
     *
     *  This function deletes the content of the provided switch structure and
     *  resets its fields using default values.
     *
     *  The function then deletes the created linked list, if present, by
     *  deleting all the engaged door structures using their specialised
     *  deletion function.
     *
     *  \param le_switch Switch structure
     */

    le_void_t le_switch_delete( le_switch_t * const le_switch );

    /*! \brief accessor methods
     *
     *  This function searches in the provided switch structure doors linked
     *  list for a door corresponding exactly to the provided time.
     *
     *  In case the corresponding door is found, it is returned as a pointer
     *  to the calling function. If the door is not found and the provided mode
     *  is set to \b LE_DOOR_WRITE, a new door is created and returned by the
     *  function after its insertion in the linked list.
     *
     *  In case of a door creation case, the specialised door creation function
     *  is used, which causes the storage structure of the door to be created.
     *
     *  \param le_switch Switch structure
     *  \param le_time   Target time
     *  \param le_mode   Storage structure access mode
     *
     *  \return Returns the found/created door structure or a NULL pointer
     */

    le_door_t * le_switch_get_inject( le_switch_t * const le_switch, le_time_t const le_time, le_enum_t const le_mode );

    /*! \brief accessor methods
     *
     *  This function searches in the provided switch structure door list the
     *  one that fits the provided query address specified time.
     *
     *  This function implements a simple switch between the two door detection
     *  methods implemented in this module which are the strict near and deep
     *  search.
     *
     *  The provided address drives the search method choice. As the subsequent
     *  method answer the found door, this function broadcast it to the calling
     *  function.
     *
     *  \param le_switch Switch structure
     *  \param le_addr   Address structure
     *  \param le_time   Address structure time index (0 or 1)
     *
     *  \return Returns a pointer to the found door structure
     */

    le_door_t * le_switch_get_query( le_switch_t const * const le_switch, le_address_t const * const le_addr, le_size_t const le_time );

    /*! \brief accessor methods
     *
     *  This function searches in the provided switch structure door list the
     *  one that fits the provided query address specified time applying a
     *  near-strict methodology, meaning that the closest door is considered
     *  even no data are available for the provided address spatial index.
     *
     *  The time range provided by the address structure is used to limit the
     *  search in the time dimension. It follows that the closest cell, in time,
     *  has to remains within this time limit in order to be returned by the
     *  function.
     *
     *  In addition, the detection of the spatial cell is performed by the
     *  function. The content of the cell is not checked by the function, the
     *  detection being mandatory for subsequent query process.
     *
     *  The function returns a pointer to the nearest door, from a temporal
     *  point of view.
     *
     *  \param le_switch Switch structure
     *  \param le_addr   Address structure
     *  \param le_time   Address structure time index (0 or 1)
     *
     *  \return Returns a pointer to the found door structure
     */

    le_door_t * le_switch_get_query_near( le_switch_t const * const le_switch, le_address_t const * const le_addr, le_size_t const le_time );

    /*! \brief accessor methods
     *
     *  This function searches in the provided switch structure door list the
     *  one that fits the provided query address specified time applying a
     *  deep methodology, meaning that doors are rejected if they contain no
     *  data according to the provided address spatial index.
     *
     *  The function then checks, using the spatial index provided through the
     *  address structure, if the door contains data accordingly. If no data are
     *  available from the spatial index point of view, the door is rejected and
     *  the door search goes on.
     *
     *  The door search is stopped under two conditions : in case the end of the
     *  linked list is reached or when the considered door is distant, from a
     *  temporal point view, of more than the time range value coming with the
     *  address structure.
     *
     *  The function implements then a dual door search : one going with time
     *  and the other against it. The function starts with the two nearest doors
     *  surrounding the target time.
     *
     *  If a relevant door is found filled with data, it is returned by the
     *  function. A NULL pointer is returned otherwise.
     *
     *  \param le_switch Switch structure
     *  \param le_addr   Address structure
     *  \param le_time   Address structure time index (0 or 1)
     *
     *  \return Returns a pointer to the found door structure, NULL otherwise
     */

    le_door_t * le_switch_get_query_deep( le_switch_t const * const le_switch, le_address_t const * const le_addr, le_size_t const le_time );

    /*! \brief mutator methods
     *
     *  This function allows the rebuild the provided switch structure doors
     *  linked list. According to the provided maximum lifespan of the switch,
     *  the function erases the linked list before to re-create it.
     *
     *  This function is used as each client connection thread are independent
     *  from each other. This allows for thread to keep track of newly injected
     *  data without having synchronise to each other directly.
     *
     *  \param le_switch   Switch structure
     *  \param le_lifespan Maximum lifetime of the switch structure, in seconds
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_switch_set_update( le_switch_t * const le_switch, le_time_t const le_lifespan );

    /*! \brief mutator methods
     *
     *  This function creates the doors linked list of the provided switch
     *  structure.
     *
     *  The function enumerates the directories of the server main storage
     *  path. Each directory is interpreted as a temporal storage unit. A door
     *  is created for each found directory.
     *
     *  As a door is created for a directory, the function calls a specialised
     *  pushing function allowing to actually create the door structure for the
     *  directory but also to insert the door structure in the correct position
     *  in the linked list.
     *
     *  \param le_switch Switch structure
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_switch_set_build( le_switch_t * const le_switch );

    /*! \brief mutator methods
     *
     *  This function allows to create and insert a new door, corresponding to
     *  a temporal unit storage directory, to the linked list of the provided
     *  switch structure.
     *
     *  The new door structure is created in the first place using the provided
     *  time linking to its storage representation. The created door is then
     *  inserted in the linked list at it corresponding position according to
     *  its time value.
     *
     *  If the provided mode value is set to \b LE_DOOR_WRITE, the door is
     *  created even its storage representation is not found, creating its
     *  storage directory.
     *
     *  The created door is then returned as a pointer after its insertion in
     *  the linked list.
     *
     *  \param le_switch Switch structure
     *  \param le_time   Door time
     *  \param le_mode   Storage structure access mode
     *
     *  \return Returns the created door structure on success, NULL otherwise
     */

    le_door_t * le_switch_set_push( le_switch_t * const le_switch, le_time_t const le_time, le_enum_t const le_mode );

    /*! \brief mutator methods
     *
     *  This function allows to deletes the doors linked list of the provided
     *  switch structure.
     *
     *  The function parses the linked list and deletes all its door using their
     *  specialised deletion function. The memory of the linked list is released
     *  along the way.
     *
     *  \param le_switch Switch structure
     */

    le_void_t le_switch_set_erase( le_switch_t * const le_switch );

    /*! \brief i/o methods
     *
     *  This function is the front-end to server parameter query from a remote
     *  client.
     *
     *  The function, after consistency checks, packs the server parameters in
     *  a socket-array that is returned to the client using the provided
     *  socket descriptor.
     *
     *  \param le_switch Switch structure
     *  \param le_array  Client socket array
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_switch_io_auth( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket );

    /*! \brief i/o methods
     *
     *  This function is the front-end to server data injection from a remote
     *  client.
     *
     *  After consistency checks, the function reads the time value packed in
     *  the provided client socket-array. The function uses this time value to
     *  query or create the appropriate door structure.
     *
     *  The gathered door is then locked before the function proceed to the
     *  injection of the data. In the first place, the client incoming data
     *  are dispatched using the door specialised function to separate the
     *  point-based data (mono-vertex) to the polygonal data (poly-vertex).
     *
     *  The function then proceed to the data injection of both group of data
     *  after their sorting. Mono-vertex data are injected in the first place,
     *  if present, before to inject the poly-vertex data, also if available.
     *
     *  At the end of the process, the door used to inject the data is unlocked
     *  on success. If a failure occurs during dispatch, sorting or injection,
     *  the door remains locked.
     *
     *  In the case of injection, as the amount of data is not known 'a priori',
     *  the reading of the incoming socket-array carrying the data is performed
     *  by the dispatch function.
     *
     *  \param le_switch Switch structure
     *  \param le_array  Client socket array
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_switch_io_inject( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket );

    /*! \brief i/o methods (outdated documentation)
     *
     *  This function is the front-end to server data query from a remote
     *  client.
     *
     *  The function allows to have multiple query address packed in the
     *  provided client socket-array. The function performs then a loop on all
     *  packed addresses, repeating the query process for each of them.
     *
     *  For each address, the function starts by checking the mode of the
     *  address to determine if a parallel query has to be performed.
     *
     *  In any cases, the function gather the required door structure to access
     *  the queried data using the address times and spatial index. The door
     *  is then used along with its specialised query function for mono-vertex,
     *  poly-vertex and mixed/parallel queries.
     *
     *  Mixed models are answered to the client as the two times of the address
     *  are active (according to the mode value) and as the data found in the
     *  door storage structure are not of the same type (mono/poly-vertex). The
     *  function implements the composition of mixed model logic.
     *
     *  The function uses the door specialised functions to gather the queried
     *  data into the provided server socket-array and returns one socket-array
     *  per address found in the client request.
     *
     *  \param le_switch Switch structure
     *  \param le_array  Client socket array
     *  \param le_socket Client socket descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_switch_io_query( le_switch_t * const le_switch, le_array_t * const le_array, le_sock_t const le_socket );

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

