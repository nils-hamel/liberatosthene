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

    /*! \file   eratosthene-address.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - address
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_ADDRESS__
    # define __LE_ADDRESS__

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
    # include "eratosthene-array.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_ADDRESS_C         { 0, 0, 0, { _LE_TIME_NULL, _LE_TIME_NULL, _LE_TIME_NULL }, { 0 } }
    # define LE_ADDRESS_C_SIZE(s) { s, 0, 0, { _LE_TIME_NULL, _LE_TIME_NULL, _LE_TIME_NULL }, { 0 } }

    /* define address convolution mode */
    # define LE_ADDRESS_FTIME     ( 1 ) /* first time only */
    # define LE_ADDRESS_STIME     ( 2 ) /* second time only */
    # define LE_ADDRESS_ACC       ( 3 ) /* accumulation */
    # define LE_ADDRESS_DDH       ( 4 ) /* differences detection */
    # define LE_ADDRESS_DDM       ( 5 ) /* differences detection with visual markers */

    /* define address query mode */
    # define LE_ADDRESS_NEAR      ( 0 )
    # define LE_ADDRESS_DEEP      ( 1 )

    /* asynchronous dimension */
    # define LE_ADDRESS_DEPTH_P   ( 1 )
    # define LE_ADDRESS_DEPTH_A   ( 10 )

    /* define WGS84 ellipsoid parameters */
    # define LE_ADDRESS_WGS_A     ( 6378137.0 )

    /* define ellipsoidal coordinates boundaries */
    # define LE_ADDRESS_MIN_L     ( - LE_PI )
    # define LE_ADDRESS_MAX_L     ( + LE_PI )
    # define LE_ADDRESS_MIN_A     ( - LE_P2 )
    # define LE_ADDRESS_MAX_A     ( + LE_P2 )
    # define LE_ADDRESS_MIN_H     ( - ( LE_PI * LE_ADDRESS_WGS_A ) / 1024.0 )
    # define LE_ADDRESS_MAX_H     ( + ( LE_PI * LE_ADDRESS_WGS_A ) / 1024.0 )

    /* define ellipsoidal coordinates ranges */
    # define LE_ADDRESS_RAN_L     ( LE_ADDRESS_MAX_L - LE_ADDRESS_MIN_L )
    # define LE_ADDRESS_RAN_A     ( LE_ADDRESS_MAX_A - LE_ADDRESS_MIN_A )
    # define LE_ADDRESS_RAN_H     ( LE_ADDRESS_MAX_H - LE_ADDRESS_MIN_H )

    /* define ellipsoidal coordinates ranges */
    # define LE_ADDRESS_IRN_L     ( 1.0 / LE_ADDRESS_RAN_L )
    # define LE_ADDRESS_IRN_A     ( 1.0 / LE_ADDRESS_RAN_A )
    # define LE_ADDRESS_IRN_H     ( 1.0 / LE_ADDRESS_RAN_H )

/*
    header - preprocessor macros
 */

    /* digit base from scale */
    # define le_address_base(s)       ( ( s ) < LE_ADDRESS_DEPTH_P ) ? ( _LE_USE_BASE >> 2 ) : ( ( ( s ) < LE_ADDRESS_DEPTH_A ) ? ( _LE_USE_BASE >> 1 ) : ( _LE_USE_BASE ) )

    /* address common digit */
    # define le_address_mac_min(s,t)  ( ( ( s )->as_size > ( t )->as_size ) ? ( t )->as_size : ( s )->as_size )

    /* method alias */
    # define le_address_get_pose(a,p) ( le_address_get_pose_( ( a ), ( a )->as_size, ( p ) ) )

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_address_struct
     *  \brief address structure (outdated documentation)
     *
     *  This structure holds the address, or the index, of an equivalence class
     *  defined on the geodetic and time parameter spaces. These addresses are
     *  the central structure used to store and access the data of a server.
     *
     *  The structure holds two distinct times in order to allows comparison
     *  methods between different times to take place. In addition to the two
     *  time values, a temporal range value is also kept by the structure that
     *  defines the area around the two previous time that has to be considered
     *  mainly during data queries. These three values are stored in the array
     *  \b as_times of the structure.
     *
     *  The mode parameter stored in \b as_mode, is used to store the way time
     *  values have to be considered. The following convolution modes are
     *  available :
     *
     *      1 : first time only
     *      2 : second time only
     *      3 : first time (logical or) second time
     *      4 : first time (logical and) second time
     *      5 : first time (logical xor) second time
     *
     *  The convolution mode is stored on the four least significant bits of the
     *  \b as_mode byte. The four most significant bits are used to store the
     *  address query mode. Two modes are available :
     *
     *      0 : strict-near search
     *      1 : deep-data search
     *
     *  The first mode indicates that the closest equivalent class has to be
     *  considered, regardless of its content. The second mode indicates that
     *  the closest non-empty equivalence class has to be considered.
     *
     *  The proper spatial index of the pointed equivalence class is stored in
     *  the array \b as_digits holding its digits. The amount of active digits
     *  is provided by the \b as_size field.
     *
     *  In the last place the structure holds an depth parameter that gives,
     *  usually in case of data queries, the additional scale depth where the
     *  desired data have to be considered/gathered, the index itself giving
     *  the base equivalence class.
     *
     *  Times are understood under the UTC standard in the way they give the
     *  amount of seconds elapsed since EPOCH (UTC) with consideration of leap
     *  seconds.
     *
     *  As an example, the following address, written in its text form :
     *
     *      /19/950486422,-7258032000/864000/122010001340232/7
     *
     *  represents the data located in Venice in 2000 and in 1740. The first
     *  number correspond to the times comparison and query modes. The two times    
     *  are given followed by the time range. The digits of the spatial address
     *  are then provided. The last number gives the additional depth where the
     *  desired data have to be searched.
     *
     *  \var le_address_struct::as_size
     *  Address number of digits
     *  \var le_address_struct::as_mode
     *  Times comparison mode
     *  \var le_address_struct::as_span
     *  Spatial index additional depth
     *  \var le_address_struct::as_times
     *  Array containing the address times and the temporal range
     *  \var le_address_struct::as_digit
     *  Array containing the address digits
     */

    typedef struct le_address_struct {

        le_byte_t as_size;
        le_byte_t as_mode;
        le_byte_t as_span;

        le_time_t as_times[_LE_USE_TIMES];
        le_byte_t as_digit[_LE_USE_DEPTH];

    } le_address_t;

/*
    header - function prototypes
 */

    /*! \brief accessor methods
     *
     *  Returns the size, ie. the number of digits, of the spatial index stored
     *  in the provided address structure.
     *
     *  \param le_address Address structure
     *
     *  \return Address spatial index number of digits
     */

    le_byte_t le_address_get_size( le_address_t const * const le_address );

    /*! \brief accessor methods
     *
     *  Returns the address times convolution mode.
     *
     *  \param le_address Address structure
     *
     *  \return Address times convolution mode
     */

    le_byte_t le_address_get_mode( le_address_t const * const le_address );

    /*! \brief accessor methods
     *
     *  Returns the address query mode.
     *
     *  \param le_address Address structure
     *
     *  \return Address query mode
     */

    le_byte_t le_address_get_query( le_address_t const * const le_address );

    /*! \brief accessor methods
     *
     *  This function returns one of the two times or the temporal range of the
     *  provided address structure.
     *
     *  The time values can be obtained by passing 0 or 1 as offset parameter.
     *  Providing 2 as offset parameter allows to obtain the temporal range
     *  stored in the structure.
     *
     *  \param le_address Address structure
     *  \param le_offset  Offset of time or temporal range - zero based
     *
     *  \return Desired address time or temporal range
     */

    le_time_t le_address_get_time( le_address_t const * const le_address, le_size_t const le_offset );

    /*! \brief accessor methods
     *
     *  Returns the desired digit of the spatial index stored in the address
     *  structure. The provided offset has to be smaller than the address size.
     *
     *  \param le_address Address structure
     *  \param le_offset  Offset of the digit - zero based
     *
     *  \return Returns address spatial index digit
     */

    le_byte_t le_address_get_digit( le_address_t const * const le_address, le_size_t const le_offset );

    /*! \brief accessor methods
     *
     *  Returns the additional depth parameter of the spatial index stored in
     *  the provided address structure.
     *
     *  \param le_address Address structure
     *
     *  \return Address spatial index additional depth
     */

    le_byte_t le_address_get_span( le_address_t const * const le_address );

    /*! \brief accessor methods
     *
     *  This function compares the two provided address structures and returns a
     *  Boolean value indicating their identity.
     *
     *  \param le_addra Address structure
     *  \param le_addrb Address structure
     *
     *  \return Returns _LE_TRUE on addresses identity, _LE_FALSE otherwise
     */

    le_enum_t le_address_get_equal( le_address_t const * const le_addra, le_address_t const * const le_addrb );

    /*! \brief accessor methods
     *
     *  This function compares the two provided address structures in terms of
     *  their spatial index digits and returns a Boolean value indicating the
     *  identity of the digits value and count.
     * 
     *  \param le_addra Address structure
     *  \param le_addrb Address structure
     *
     *  \return Returns _LE_TRUE on index digits identity, _LE_FALSE otherwise
     */

    le_enum_t le_address_get_equal_index( le_address_t const * const le_addra, le_address_t const * const le_addrb );

    /*! \brief accessor methods (detached)
     *
     *  Note :
     *
     *  Detached methods are specific functions associated to a structure but
     *  without operating directly on it. Such function are used in optimisation
     *  strategies.
     *
     *  This detached methods directly compares the two provided geographic
     *  positions from a spatial index point of view. The implicit digits of the
     *  two positions are computed and compared to check which position lead to
     *  a spatial index considered as greater.
     *
     *  If the first position lead to a greater or equal spatial index according
     *  to the second one, the function returns \b _LE_TRUE, \b _LE_FALSE in the
     *  other case.
     *
     *  The \b le_size parameter has to give the size of the spatial index to
     *  consider for comparison. This size is the implicit size of the spatial
     *  index computed for each position.
     *
     *  \param le_fpose Pointer to the first position array
     *  \param le_spose Pointer to the second position array
     *  \param le_size  Implicit size of the subsequent address structure
     *
     *  \return Returns _LE_TRUE if the first position lead to a greater or
     *  equal spatial index, _LE_FALSE otherwise
     */

    le_enum_t le_address_get_greater( le_real_t const * const le_fpose, le_real_t const * const le_spose, le_size_t const le_size );

    /*! \brief accessor methods
     *
     *  This function compute the spatial index distance between the two
     *  provided addresses. The distance is understood as the lower index at
     *  which the two address spatial index expose a different digit.
     *
     *  The provided size is considered as the common digit number to consider
     *  for the distance computation. This size overrides the two sizes coming
     *  with the provided address structures.
     *
     *  \param le_address Address structure
     *  \param le_origin  Address structure
     *  \param le_size    Implicit size of the common address
     *
     *  \return Returns the deepest scale at which a digit differ in the two
     *  addresses spatial index
     */

    le_size_t le_address_get_dist( le_address_t const * const le_address, le_address_t const * const le_origin, le_size_t const le_size );

    /*! \brief accessor methods
     *
     *  This function reads the digits of the index stored in the address
     *  structure and converts it into a geographic coordinates 3-vector.
     *
     *  In other words, this function computes the geographic coordinates of the
     *  equivalence class corner pointed by the index stored in the address
     *  structure.
     *
     *  This function expects the number of digits of the index to be provided
     *  as parameter, discarding the size field of the address structure. This
     *  allows specific optimisation to take place.
     *
     *  The \b le_address_get_pose() macro provides a way to invoke the function
     *  without having to explicitly pass the address size as parameter.
     *
     *  \param le_address Address structure
     *  \param le_size    Number of digits to consider
     *  \param le_pose    3-vector receiving the coordinates
     */

    le_void_t le_address_get_pose_( le_address_t const * const le_address, le_size_t le_size, le_real_t * const le_pose );

    /*! \brief mutator methods
     *
     *  This function allows to set the number of active digit of the spatial
     *  index of the provided address structure.
     *
     *  \param le_address Address structure
     *  \param le_size    Number of digits
     */

    le_void_t le_address_set_size( le_address_t * const le_address, le_byte_t const le_size );

    /*! \brief mutator methods
     *
     *  Sets the provided address times convolution mode.
     *
     *  \param le_address Address structure
     *  \param le_mode    Times convolution mode
     */

    le_void_t le_address_set_mode( le_address_t * const le_address, le_byte_t const le_mode );

    /*! \brief mutator methods
     *
     *  Sets the provided address query mode.
     *
     *  \param le_address Address structure
     *  \param le_query   Query mode
     */

    le_void_t le_address_set_query( le_address_t * const le_address, le_byte_t const le_query );

    /*! \brief mutator methods
     *
     *  This function allows to override the value of the two times or the value
     *  of the temporal range of the provided address structure.
     *
     *  If the provided offset is set to 0 or 1, the corresponding time value is
     *  replaced. If 2 is provided as offset, the temporal range is replaced
     *  with the provided time value.
     *
     *  \param le_address Address structure
     *  \param le_offset  Offset of time or temporal range - zero based
     *  \param le_time    Time or temporal range value
     */

    le_void_t le_address_set_time( le_address_t * const le_address, le_size_t const le_offset, le_time_t const le_time );

    /*! \brief mutator methods
     *
     *  Sets the specified digit of the spatial index stored in the provided
     *  address structure. The provided offset has to be a whole number smaller
     *  than the address size.
     *
     *  \param le_address Address structure
     *  \param le_offset  Offset of the digit - zero based
     *  \param le_digit   Digit value
     */

    le_void_t le_address_set_digit( le_address_t * const le_address, le_size_t const le_offset, le_byte_t const le_digit );

    /*! \brief mutator methods
     *
     *  Sets the provided address spatial index additional depth value.
     *
     *  \param le_address Address structure
     *  \param le_span    Additional depth
     */

    le_void_t le_address_set_span( le_address_t * const le_address, le_byte_t const le_span );

    /*! \brief mutator methods
     *
     *  This function sets the digits of the index stored in the address
     *  structure induced by the provided 3-vector expressed in geographic
     *  coordinates.
     *
     *  The number of digits is read in the structure itself and has to be set
     *  before any call to this function.
     *
     *  \param le_address Address structure
     *  \param le_pose    3-vector containing the coordinates
     */

    le_void_t le_address_set_pose( le_address_t * const le_address, le_real_t const * const le_pose );

    /*! \brief serialisation method
     *
     *  This functions is used to pack and unpack an address structure in and
     *  from the provided socket-array. In other words, this function ensure the
     *  translation between the structure memory representation and its packed
     *  representation in a bytes array.
     *
     *  The provided \b le_offset parameter indicates at which byte position the
     *  packing or unpacking of the structure has to take place in the provided
     *  socket-array.
     *
     *  The provided socket-array, for both packing and unpacking, as to be an
     *  already allocated array allowing the packing and unpacking to take
     *  place.
     *
     *  \param le_address Address structure
     *  \param le_array   Array structure
     *  \param le_offset  Serialisation offset in array, in bytes
     *  \param le_mode    Serialisation mode (_LE_SET or _LE_GET)
     *
     *  \return Offset of the byte following the structure in the socket-array
     */

    le_size_t le_address_serial( le_address_t * const le_address, le_array_t * const le_array, le_size_t const le_offset, le_enum_t const le_mode );

    /*! \brief conversion methods
     *
     *  This function translates the address structure content in human readable
     *  text form.
     *
     *  Considering m the times comparison mode, t1 and t2 the times, r the
     *  temporal range, d the  digits of the spatial index and q the spatial
     *  index additional depth, the function provides a string structured as
     *  follows :
     *
     *      /m/t1,t2/r/ddd...d/q
     *
     *  \param le_address Address structure
     *  \param le_string  String receiving the address text representation
     */

    le_void_t le_address_ct_string( le_address_t const * const le_address, le_char_t * const le_string );

    /*! \brief conversion methods
     *
     *  This function inverts the conversion made by \b le_address_ct_string()
     *  function. It expects an address in human readable text and converts it
     *  in an address structure.
     *
     *  \param le_address Address structure
     *  \param le_string  String containing the address text representation
     */

    le_void_t le_address_cf_string( le_address_t * const le_address, le_char_t const * const le_string );

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

