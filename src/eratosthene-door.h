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

    /*! \file   eratosthene-door.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - door
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_DOOR__
    # define __LE_DOOR__

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
    # include "eratosthene-class-mono.h"
    # include "eratosthene-class-poly.h"
    # include "eratosthene-geodesy.h"
    # include "eratosthene-operator.h"
    # include "eratosthene-uv3.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_DOOR_C            { { 0 }, { 0 }, 0, 0, 0, 0, LE_DOOR_CLOSE, { NULL }, _LE_OFFS_NULL, { NULL }, _LE_OFFS_NULL, NULL, NULL, NULL, LE_ERROR_SUCCESS }

    /* define pseudo-constructor */
    # define LE_DOOR_C_SCT(s,c,t) { { 0 }, { 0 }, 0, s, c, t, LE_DOOR_CLOSE, { NULL }, _LE_OFFS_NULL, { NULL }, _LE_OFFS_NULL, NULL, NULL, NULL, LE_ERROR_SUCCESS }

    /* define door link */
    # define LE_DOOR_PREV         ( 0 )
    # define LE_DOOR_NEXT         ( 1 )

    /* define door mode */
    # define LE_DOOR_READ         ( 0 )
    # define LE_DOOR_WRITE        ( 1 )

    /* define door state */
    # define LE_DOOR_LOCK         ( 0 )
    # define LE_DOOR_UNLOCK       ( 1 )

    /* define door stream */
    # define LE_DOOR_CLOSE        ( 0 )
    # define LE_DOOR_OPEN         ( 1 )

/*
    header - preprocessor macros
 */

    /* stream path tail */
    #define le_door_mac_tail( d, o, c ) ( d )->dr_strp[( d )->dr_plen + ( o )] = ( char ) ( c )

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_door_struct
     *  \brief door structure
     *
     *  This structure holds the required information and data for accessing
     *  the storage structure of a server time unit.
     *
     *  The three first fields store the path of the time unit, a copy if this
     *  path and the length of the path itself. The path copy is used as a
     *  dynamic path that is completed for data access while the path itself
     *  stays unchanged in the first string.
     *
     *  The two next fields store a copy of the server space and time parameters
     *  in order for the structure to properly access the data.
     *
     *  The next field stores the time value of the time unit. The time unit
     *  is stored in its reduced form, that is the proper time divided by the
     *  value of the server time parameter.
     *
     *  The next field holds the stream state flag. It indicates if the stream
     *  used to access the door storage representation are open or closed.
     *
     *  The next two fields, an array and a size, are used to access mono-vertex
     *  related data in the storage structure. The array is used to store the
     *  stream descriptor opened toward the files of the storage structure. The
     *  amount of descriptors corresponds then to the amount of scale of the
     *  server, that is its space parameter value. The offset field coming after
     *  the array is used during data access to keep track of the offset, in a
     *  specific scale, of the data that have to be accessed.
     *
     *  The next three fields are similar than the previous one but for the case
     *  of poly-vertex access. In addition to the stream descriptors stack and
     *  tracking offset, poly-vertex have to maintain an additional descriptor
     *  toward the file containing the poly-vertex actual definition.
     *
     *  The two last field are used to create and maintain linked list. The two
     *  pointers are used to point at the previous and next door structure
     *  engaged in the linked list, NULL being used as a terminal signal, in
     *  both directions.
     *
     *  The storage structure of a door is structured into three directories :
     *
     *      /.../door_reduced_time/0
     *      /.../door_reduced_time/1
     *      /.../door_reduced_time/2
     *
     *  The directory 0 is used for the client data reception. Its role is to
     *  hold the dispatched mono and poly-vertex data chunks. The data are kept
     *  in small chunks named 10, 11, 12, ... for the mono-vertex and 20, 21,
     *  22, ... for the poly-vertex. The 0 directory is also used during chunks
     *  merge processes.
     *
     *  The tree structure for mono-vertex is created and hold in the directory
     *  1. This directory contains a binary file per scale engaged in the server
     *  configuration.
     *
     *  The tree structure for poly-vertex is created and hold in the directory
     *  2. This directory contains also a binary file per scale engaged in the
     *  server configuration.
     *
     *  To manage successive and distinct data injection processes into the same
     *  door structure, the results of the merging of the dispatched chunks are
     *  kept in the door main directory. The file is named 1_ for mono-vertex
     *  and 2_ for poly-vertex. These files are kept to be able to merge them
     *  with the result of the incoming new data through successive injections
     *  to always keep a single sorted uv3 file containing all the injected
     *  data. This allows to use transverse injection processes in any cases,
     *  leading to more efficient data management.
     *
     *  \var le_door_struct::dr_path
     *  Path of the time unit directory
     *  \var le_door_struct::dr_strp
     *  Path of the time unit directory - copy
     *  \var le_door_struct::dr_plen
     *  Length of the path of the time unit directory
     *  \var le_door_struct::dr_scfg
     *  Server spatial configuration parameter - copy
     *  \var le_door_struct::dr_tcfg
     *  Server temporal configuration parameter - copy
     *  \var le_door_struct::dr_time
     *  Door unit time value, in reduced form
     *  \var le_door_struct::dr_mode
     *  State of the door access streams (open/close)
     *  \var le_door_struct::dr_macc
     *  Stream stack toward mono-vertex tree structure
     *  \var le_door_struct::dr_moff
     *  Mono-vertex offset tracker
     *  \var le_door_struct::dr_pacc
     *  Stream stack toward poly-vertex tree structure
     *  \var le_door_struct::dr_poff
     *  Poly-vertex offset tracker
     *  \var le_door_struct::dr_pdat
     *  Poly-vertex storage stream
     *  \var le_door_struct::dr_prev
     *  Linked list pointer (previous)
     *  \var le_door_struct::dr_next
     *  Linked list pointer (next)
     *  \var le_door_struct::_status
     *  Standard status field
     */

    typedef struct le_door_struct {

        le_char_t   dr_path[_LE_USE_PATH];
        le_char_t   dr_strp[_LE_USE_PATH];
        le_size_t   dr_plen;

        le_size_t   dr_scfg;
        le_time_t   dr_tcfg;
        le_time_t   dr_time;
        le_enum_t   dr_mode;

        le_file_t   dr_macc[_LE_USE_DEPTH];
        le_size_t   dr_moff;

        le_file_t   dr_pacc[_LE_USE_DEPTH];
        le_size_t   dr_poff;
        le_file_t   dr_pdat;

        le_void_t * dr_prev;
        le_void_t * dr_next;

    le_enum_t _status; } le_door_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function creates a door structure toward the time unit pointed by
     *  the provided time value.
     *
     *  After the copy of the provided path and parameters, the function checks
     *  the door creation mode.
     *
     *  If the mode \b LE_DOOR_WRITE is provided, the function checks if the
     *  storage of the temporal unit associated with the created door already
     *  exists. In such case, an error is raised.
     *
     *  If the storage does not exist, the function creates it and initialise
     *  its content in terms of files and directory.
     *
     *  This function returning the created structure, the status is stored in
     *  the structure itself using the reserved \b _status field.
     *
     *  \param le_root Server storage structure path
     *  \param le_scfg Server spatial parameter
     *  \param le_tcfg Server temporal parameter
     *  \param le_time Temporal unit time
     *  \param le_mode Structure creation mode (write/read)
     *
     *  \return Returns the created structure
     */

    le_door_t le_door_create( le_char_t const * const le_root, le_size_t const le_scfg, le_time_t const le_tcfg, le_time_t const le_time, le_enum_t const le_mode );

    /*! \brief constructor/destructor methods
     *
     *  This function is used to delete the content of the provided door
     *  structure. The fields of the provided door structure are simply erased
     *  using default values.
     *
     *  The state of the access stream is not checked by the function. It
     *  follows that the function assumes that the stream have been already
     *  been closed by previous processes.
     *
     *  \param le_door Door structure
     */

    le_void_t le_door_delete( le_door_t * const le_door );

    /*! \brief accessor methods
     *
     *  This function allows to obtain the time of the door structure in its
     *  reduce form, that is divided by the server temporal parameter.
     *
     *  \param le_door Door Structure
     *
     *  \return Return structure reduced time
     */

    le_time_t le_door_get_reduced( le_door_t const * const le_door );

    /*! \brief accessor methods
     *
     *  This function returns the previous door structure of the first provided
     *  structure or the next one of the second provided structure according to
     *  their proximity to the provided time value.
     *
     *  The function performs a temporal proximity checks between the two door
     *  structures candidates before to return the \b LE_DOOR_NEXT value or the
     *  \b LE_DOOR_PREV value according to the result.
     *
     *  The provided time value has to be given in its reduced form.
     *
     *  \param le_prev    Door structure
     *  \param le_next    Door structure
     *  \param le_reduced Time value, in reduced form
     *
     *  \return Returns LE_DOOR_PREV or LE_DOOR_NEXT according to the proximity
     *  to the time value
     */

    le_enum_t le_door_get_nearest( le_door_t const * const le_prev, le_door_t const * const le_next, le_time_t const le_reduced );

    /*! \brief accessor methods
     *
     *  This function returns a Boolean value according to the state of the
     *  provided door structure tracking offset for mono-vertex.
     *
     *  If the door tracking offset points to a valid location, the function
     *  returns _LE_TRUE, _LE_FALSE otherwise.
     *
     *  \param le_door Door structure
     *
     *  \return Returns _LE_TRUE if the structure mono-vertex tracking offset
     *  points to a valid location, _LE_FALSE otherwise
     */

    le_enum_t le_door_get_mono( le_door_t const * const le_door );

    /*! \brief accessor methods
     *
     *  This function returns a Boolean value according to the state of the
     *  poly-vertex tracking offset of the provided door structure.
     *
     *  If the door tracking offset points to a valid location, the function
     *  returns _LE_TRUE, _LE_FALSE otherwise.
     *
     *  \param le_door Door structure
     *
     *  \return Returns _LE_TRUE if the structure poly-vertex tracking offset
     *  points to a valid location, _LE_FALSE otherwise
     */

    le_enum_t le_door_get_poly( le_door_t const * const le_door );

    /*! \brief accessor methods
     *
     *  This function returns a Boolean value if the provided time is in the
     *  equivalence class of the provided door structure. The time value has to
     *  be in the server time frame (not in reduced form).
     *
     *  \param le_door Door structure
     *  \param le_time Time value
     *
     *  \return Returns _LE_TRUE if the provided time value belong to the
     *  structure temporal equivalence class.
     */

    le_enum_t le_door_get_equal( le_door_t const * const le_door, le_time_t const le_time );

    /*! \brief accessor methods
     *
     *  This function returns \b _LE_TRUE Boolean value if the provided door
     *  structure is greater than the provided door candidate, \b _LE_FALSE
     *  otherwise. The comparison is made from the temporal point of view using
     *  the time value of the provided structure.
     *
     *  \param le_door      Door structure
     *  \param le_candidate Door structure
     *
     *  \return Returns _LE_TRUE if the candidate is anterior to the provided
     *  door structure from a temporal point of view
     */

    le_enum_t le_door_get_anterior( le_door_t const * const le_door, le_door_t const * const le_candidate );

    /*! \brief accessor methods
     *
     *  This function simply returns the pointer to the previous door structure
     *  engaged in the linked list of the provided one.
     *
     *  \param le_door Door structure
     *
     *  \return Returns the previous door structure in the linked list
     */

    le_door_t * le_door_get_prev( le_door_t const * const le_door );

    /*! \brief accessor methods
     *
     *  This function simply returns the pointer to the next door structure
     *  engaged in the linked list of the provided one.
     *
     *  \param le_door Door structure
     *
     *  \return Returns the next door structure in the linked list
     */

    le_door_t * le_door_get_next( le_door_t const * const le_door );

    /*! \brief accessor methods
     *
     *  This function checks if any chunks of dispatched data are present in the
     *  provided door storage structure for the specified type of data.
     *
     *  If 1 is provided as suffix value, the checks is performed on mono-vertex
     *  data. If 2 is provided, the check is made on the poly-vertex data.
     *
     *  In any case, if at least one chunk of data is detected for the specified
     *  type, the function returns \b _LE_TRUE, \b _LE_FALSE otherwise.
     *
     *  \param le_door   Door structure
     *  \param le_suffix Data type suffix
     *
     *  \return Returns _LE_TRUE if a data chunk is detected, _LE_FALSE
     *  otherwise
     */

    le_enum_t le_door_get_dispatch( le_door_t const * const le_door, le_size_t const le_suffix );

    /*! \brief mutator methods
     *
     *  This function allows to initialise the content of the storage structure
     *  of a temporal unit.
     *
     *  It creates the three needed directories (0,1,2) and initialises the
     *  files in the 1 and 2 directories in which data are written for mono- and
     *  poly-vertex, respectively.
     *
     *  \param le_door Door structure
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_door_set_bootstrap( le_door_t * const le_door );

    /*! \brief mutator methods
     *
     *  This function creates or deletes the stream descriptors needed to access
     *  the files of the provided door structure linked to a temporal unit
     *  storage.
     *
     *  If the provided mode is \b LE_DOOR_OPEN, the function creates the needed
     *  stream descriptor towards the storage files of the temporal unit. If a
     *  stream descriptor creation fails, all other descriptors are deleted.
     *
     *  If the provided mode is \b LE_MODE_CLOSE, the function deletes all the
     *  access stream of the provided door structure. The state of each stream
     *  is tested to only close formally open access stream.
     *
     *  \param le_door Door structure
     *  \param le_mode Stream mode
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise
     */

    le_enum_t le_door_set_stream( le_door_t * const le_door, le_enum_t const le_mode );

    /*! \brief mutator methods
     *
     *  This function allows to resets the files in which data are stored in
     *  order to reset a temporal unit storage.
     *
     *  Depending on the provided type, 1 for mono-vertex and 2 for poly-vertex,    
     *  the function resets all files to a length of zero, clearing all their
     *  content.
     *
     *  This function only resets the files associated to the scales of the
     *  storage structure.
     *
     *  \param le_door Door structure
     *  \param le_type Stream type
     *
     *  \return Returns _LE_TRUE on success, _LE_FALSE otherwise    
     */

    le_enum_t le_door_set_reset( le_door_t * const le_door, le_size_t const le_type );

    /*! \brief mutator methods
     *
     *  This function allows to update the storage structure state pointed by
     *  the provided door structure. If \b LE_DOOR_LOCK is provided, the door
     *  storage structure is locked, indicating other processes that it can not
     *  be access for writing. If \b LE_DOOR_UNLOCK is provided, the function
     *  unlock the door storage structure.
     *
     *  \param le_door  Door structure
     *  \param le_state Lock state
     *
     *  \return Returns _LE_TRUE if the state was able to be updated, _LE_FALSE
     *  otherwise
     */

    le_enum_t le_door_set_state( le_door_t const * const le_door, le_enum_t const le_state );

    /*! \brief mutator methods
     *
     *  This function allows to insert the provided door structure between the
     *  two other provided structure in the linked list they define. The links
     *  of the provided structure are updated along with the links of the two
     *  other structures of the linked list.
     *
     *  \param le_door Door structure
     *  \param le_prev Previous door structure in the linked list
     *  \param le_next Next door structure in the linked list
     */

    le_void_t le_door_set_insert( le_door_t * const le_door, le_door_t * const le_prev, le_door_t * const le_next );

    /*! \brief i/o methods
     *
     *  This function is responsible of receiving the injected data from a
     *  connected client.
     *
     *  This function performs two important writing operations on the provided
     *  door storage structure.
     *
     *  The first operation is to dispatch the incoming data into two groups of
     *  chunks that are mono-vertex and poly-vertex chunks. It continues the
     *  dispatch until the connected client stops sending data.
     *
     *  The second operation the function performs is the sorting of the chunks
     *  of each of the two groups, mono-vertex and poly-vertex. Each groups of
     *  data is dispatched in chunks of a small and controlled size. As one
     *  chunk reaches the limiting size, the function sorts its content before
     *  to write it in the provided door storage structure.
     *
     *  The sorting of the chunks is performed considering a merge-sort process
     *  with a comparison function based on the spatial index of the data
     *  positions coordinates using uv3 specialised function.
     *
     *  At the end of the process performed by the function, the door storage
     *  structure ends with two groups of files containing each the dispatched
     *  an sorted data content for the mono-vertex and poly-vertex.
     *
     *  The input data are expected to be in uv3 format. The output data are
     *  kept in the same format.
     *
     *  Because the function writes on the door storage structure, the function
     *  assumes that the door has been locked.
     *
     *  \param le_door   Door structure
     *  \param le_array  Socket-array used for client data reading
     *  \param le_socket Socket of the connected client
     *
     *  \return Return LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_door_io_each_inject_dispatch( le_door_t const * const le_door, le_array_t * const le_array, le_sock_t const le_socket );

    /*! \brief i/o methods
     *
     *  This function is responsible for the merging of the chunks created
     *  during the injection process by the \b le_door_io_each_inject_dispatch()
     *  function.
     *
     *  The function reads the chunks of the specified group, using the suffix
     *  parameter, and merge them into a single chunks. The merging of the
     *  chunks can be seen as the last steps of a global merge-sort initiated
     *  by the dispatch function.
     *
     *  The sorting of the chunks into a single file is made considering a merge
     *  sort based on the spatial index of the element positions contained in
     *  the pre-processed chunks. The merging is performed considering the uv3
     *  specialised function.
     *
     *  For both mono and poly-vertex data, a last merge operation is performed
     *  in the case previous data have already been injection in the door
     *  structure in the past. This allows to re-create the tree structure
     *  taking into account both new and old data. The resulting chunk of all
     *  the merging operation is kept in the door storage structure.
     *
     *  The input data are expected to be in uv3 format. The output data are
     *  kept in the same format.
     *
     *  Because the function writes on the door storage structure, the function
     *  assumes that the door has been locked.
     *
     *  \param le_door   Door structure
     *  \param le_suffix Data type suffix
     *
     *  \return Return LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_door_io_each_inject_merge( le_door_t const * const le_door, le_size_t const le_suffix );

    /*! \brief i/o methods
     *
     *  This function creates the storage tree structure using the sorted and
     *  merged files of the data injected by a client for mono-vertex data. This
     *  function is usually placed after the injection dispatch and merging.
     *
     *  The function reads the mono-vertex sorted data and creates the tree
     *  structure using a transverse approach (in opposition to tree-driven
     *  writing process). This allows the function to writes tree structure in
     *  an efficient way.
     *
     *  For mono-vertex data, the only data needed to represent a point in the
     *  equivalence class structure is only a color that is packed within the
     *  tree along with the linking offsets.
     *
     *  The input data are expected to be in uv3 format.
     *
     *  Because the function writes on the door storage structure, the function
     *  assumes that the door has been locked.
     *
     *  \param le_door Door structure
     *
     *  \return Return LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_door_io_mono_inject( le_door_t const * const le_door );

    /*! \brief i/o methods (Patched - need reformulation/re-implementation) (revoked) (reformulated - patch removed)
     *
     *  This function creates the storage tree structure using the sorted and
     *  merged files of the data injected by a client for poly-vertex data. This
     *  function is usually placed after the injection dispatch and merging.
     *
     *  The function reads the poly-vertex sorted data and creates the tree
     *  structure using a transverse approach (in opposition to tree-driven
     *  writing process). This allows the function to writes tree structures in
     *  an efficient way.
     *
     *  For poly-vertex, the definition of the graphical primitives are kept in
     *  the injection sorted and merged file. The tree structure holds links to
     *  this file along with the linking offset.
     *
     *  The input data are expected to be in the uv3 formats.
     *
     *  Because the function writes on the door storage structure, the function
     *  assumes that the door has been locked.
     *
     *  \param le_door Door structure
     *
     *  \return Return LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_door_io_poly_inject( le_door_t const * const le_door );

    /*! \brief i/o methods
     *
     *  This function searches the offset of the spatial equivalence class that
     *  is pointed by the index hold in the provided address structure for
     *  mono-vertex.
     *
     *  The function follows the tree structure links driven by the spatial
     *  index to determine if any data can be found in the pointed class. The
     *  function returns \b _LE_TRUE if the class is found, and \b _LE_FALSE
     *  in the other case, indicating that no data can be found further away.
     *
     *  This function is used before to perform any query over the data hold in
     *  the mono-vertex tree structure. In the case the class is found, it
     *  updates the tracking offset of the provided door structure for the
     *  subsequent gathering processes.
     *
     *  \param le_door Door structure
     *  \param le_addr Address structure
     *
     *  \return Returns _LE_TRUE if the class pointed by the address exists,
     *  _LE_FALSE otherwise.
     */

    le_enum_t le_door_io_mono_detect( le_door_t * const le_door, le_address_t const * const le_addr );

    /*! \brief i/o methods
     *
     *  This function searches the offset of the spatial equivalence class that
     *  is pointed by the index hold in the provided address structure for
     *  poly-vertex.
     *
     *  The function follows the tree structure links driven by the spatial
     *  index to determine if any data can be found in the pointed class. The
     *  function returns \b _LE_TRUE if the class is found, and \b _LE_FALSE
     *  in the other case, indicating that no data can be found further away.
     *
     *  This function is used before to perform any query over the data hold in
     *  the poly-vertex tree structure. In the case the class is found, it
     *  updates the tracking offset of the provided door structure for the
     *  subsequent gathering processes.
     *
     *  \param le_door Door structure
     *  \param le_addr Address structure
     *
     *  \return Returns _LE_TRUE if the class pointed by the address exists,
     *  _LE_FALSE otherwise.
     */

    le_enum_t le_door_io_poly_detect( le_door_t * const le_door, le_address_t const * const le_addr );

    /*! \brief i/o methods
     *
     *  This function performs the data gathering on mono-vertex tree structure
     *  for data queries. The gathered data are packed in the provided array
     *  structure following the uv3 format.
     *
     *  This function assumes that the class on which the gathering process
     *  exists and is pointed by the door structure mono-vertex tracking
     *  offset. Starting with this offset, the function enumerates all existing
     *  daughter classes to gather the data in the scale pointed by the span
     *  value. The provided address structure is used as the enumeration
     *  backbone through its digits array.
     *
     *  The enumeration is performed in a recursive way by enumerating all links
     *  of all subsequent classes and following them. The process is maintained
     *  until the targeted scale is reached.
     *
     *  For mono-vertex, the position of the data element is deduced from the
     *  backbone address structure. The color of the elements is directly
     *  extracted from the tree structure.
     *
     *  If the provided array is not empty, its content is kept and the gathered
     *  data are packed after the existing ones.
     *
     *  \param le_door  Door structure
     *  \param le_addr  Address structure
     *  \param le_parse Current enumerated scale
     *  \param le_span  Target scale
     *  \param le_array Array structure
     */

    le_void_t le_door_io_mono_gather( le_door_t * const le_door, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array );

    /*! \brief i/o methods
     *
     *  This function performs the data gathering on poly-vertex tree structure
     *  for data queries. The gathered data are packed in the provided array
     *  structure following the uv3 format.
     *
     *  This function assumes that the class on which the gathering process
     *  exists and is pointed by the door structure poly-vertex tracking
     *  offset. Starting with this offset, the function enumerates all existing
     *  daughter classes to gather the data in the scale pointed by the span
     *  value. The provided address structure is used as the enumeration
     *  backbone through its digits array.
     *
     *  The enumeration is performed in a recursive way by enumerating all links
     *  of all subsequent classes and following them. The process is maintained
     *  until the targeted scale is reached.
     *
     *  For poly-vertex, the tree structure contains links to the elements
     *  stored in the injection sorted and merged file. This file is then read,
     *  following the found links, to extract the elements definition to pack
     *  them in the provided array structure.
     *
     *  If the provided array is not empty, its content is kept and the gathered
     *  data are packed after the existing ones.
     *
     *  \param le_door  Door structure
     *  \param le_addr  Address structure
     *  \param le_parse Current enumerated scale
     *  \param le_span  Target scale
     *  \param le_array Array structure
     */

    le_void_t le_door_io_poly_gather( le_door_t * const le_door, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array );

    /*! \brief i/o methods - missing documentation */

    le_void_t le_door_io_mono_dd( le_door_t * const le_pdoor, le_door_t * const le_sdoor, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array, le_size_t le_delay );

    /*! \brief i/o methods - missing documentation */

    le_void_t le_door_io_poly_dd( le_door_t * const le_pdoor, le_door_t * const le_sdoor, le_address_t * const le_addr, le_size_t const le_parse, le_size_t const le_span, le_array_t * const le_array, le_array_t * const le_dual, le_array_t * const le_beacon );

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

