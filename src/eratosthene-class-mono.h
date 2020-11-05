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

    /*! \file   eratosthene-class-mono.h
     *  \author Nils Hamel <nils.hamel@bluewin.ch>
     *
     *  liberatosthene - class-mono (mono-vertex class)
     */

/*
    header - inclusion guard
 */

    # ifndef __LE_CLASS_MONO__
    # define __LE_CLASS_MONO__

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
    # include "eratosthene-class.h"

/*
    header - external includes
 */

/*
    header - preprocessor definitions
 */

    /* define pseudo-constructor */
    # define LE_MONO_C      { 0, { 0.0 }, { 0 } }

    /* define class structure */
    # define LE_MONO_HEADER ( sizeof( le_byte_t ) * 3 )

    /* define class structure */
    # define LE_MONO_EXTEND ( LE_MONO_HEADER + LE_CLASS_HEADER )

    /* define class structure */
    # define LE_MONO_CLASS  ( LE_CLASS_MEMORY )

    /* define class memory */
    # define LE_MONO_MEMORY ( LE_MONO_HEADER + LE_MONO_CLASS )

/*
    header - preprocessor macros
 */

/*
    header - type definition
 */

/*
    header - structures
 */

    /*! \struct le_mono_struct
     *  \brief mono-vertex class structure
     *
     *  This structure holds the definition and required elements to access mono
     *  vertex storage representation in door tree structure.
     *
     *  Three main elements are hold by the structure and its tree structure
     *  storage representation.
     *
     *  The first element is the color of the element injected in the tree
     *  structure. The color are three bytes hold in the first positions of the
     *  storage representation \b mc_data.
     *
     *  The next main element is the implicit class structure to gives access
     *  to the daughter mono-vertex classes links in the scale just below. These
     *  links are simple offset managed by the implicit class structure.
     *
     *  The storage representation of one mono-vertex class can then be seen as
     *  follows :
     *
     *      [r][g][b][implicit class bytes]
     *
     *  The last main element of the structure is the color accumulation array
     *  and its corresponding size. As many point can be injected in the same
     *  class, a mean color has to be computed and assigned in the class storage
     *  representation. The \b mc_push array and \b mc_size counter are used
     *  during injection process to compute the mean color.
     *
     *  \var le_mono_struct::mc_size
     *  Mean color accumulation counter
     *  \var le_mono_struct::mc_push
     *  Mean color component accumulation values
     *  \var le_mono_struct::mc_data
     *  Buffer used to read storage class representation
     */

    typedef struct le_mono_struct {

        le_size_t mc_size;
        le_real_t mc_push[3];
        le_byte_t mc_data[LE_MONO_MEMORY];

    } le_mono_t;

/*
    header - function prototypes
 */

    /*! \brief constructor/destructor methods
     *
     *  This function creates and initialises the content of a mono-vertex class
     *  structure.
     *
     *  The function initialises the structure fields with default values and
     *  call the initialisation function of its implicit class structure.
     *
     *  \return Returns created structure
     */

    le_mono_t le_mono_create( le_void_t );

    /*! \brief constructor/destructor methods
     *
     *  This function allows to reset the provided mono-vertex class structure
     *  to restore its state as it would have been created by the pseudo
     *  constructor function \b le_mono_create().
     *
     *  \param le_mono Mono-vertex class structure
     */

    le_void_t le_mono_reset( le_mono_t * const le_mono );

    /*! \brief constructor/destructor methods
     *
     *  This function deletes the provided mono-vertex class structure. It
     *  simply resets the structure fields using default values.
     *
     *  \param le_mono Mono-vertex class structure
     */

    le_void_t le_mono_delete( le_mono_t * const le_mono );

    /*! \brief accessor methods
     *
     *  This function allows to query the offset value stored in the provided
     *  mono-vertex class structure. The provided index indicates which offset
     *  to return and has to be in the [0,7] range.
     *
     *  The function uses the specialised implicit class function to access the
     *  desired offset.
     *
     *  \param le_mono  Mono-vertex class structure
     *  \param le_index Offset index
     *
     *  \return Returns the extracted offset from the class structure
     */

    le_size_t le_mono_get_offset( le_mono_t const * const le_mono, le_size_t const le_index );

    /*! \brief mutator methods
     *
     *  This function allows to update the offset of the provided mono-vertex
     *  class structure using the provided value. The index indicates which
     *  offset to update and has to be in the [0,7] range.
     *
     *  The function uses the specialised implicit class function to update the
     *  desired offset.
     *
     *  \param le_mono   Mono-vertex class structure
     *  \param le_index  Offset index
     *  \param le_offset Offset value
     */

    le_void_t le_mono_set_offset( le_mono_t * const le_mono, le_size_t const le_index, le_size_t const le_offset );

    /*! \brief mutator methods
     *
     *  This function allows to push the color of an element in the accumulation
     *  array of the provided mono-vertex class structure. The color is pushed
     *  on the class accumulation array and the counter is updated.
     *
     *  This function is mainly used during data injection processes to compute
     *  the representative color of an equivalence class in the case multiple
     *  elements are present.
     *
     *  \param le_mono Mono-vertex class structure
     *  \param le_data Pointer to the color information, in 8-bits RGB
     */

    le_void_t le_mono_set_push( le_mono_t * const le_mono, le_byte_t const * const le_data );

    /*! \brief i/o methods
     *
     *  This function is used to read the class representation from a storage
     *  tree structure in the provided mono-vertex class structure. The function
     *  reads the entire representation in the storage structure in one single
     *  reading, including the color information and its implicit class
     *  structure.
     *
     *  The provided offset value indicates where to start the representation
     *  reading in the provided stream. The provided stream has to be an already
     *  opened stream in binary read mode.
     *
     *  \param le_mono   Mono-vertex class structure
     *  \param le_offset Class reading offset, in bytes
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_mono_io_read( le_mono_t * const le_mono, le_size_t const le_offset, le_file_t const le_stream );

    /*! \brief i/o methods
     *
     *  This function allows to read the content of a mono-vertex class storage
     *  representation but only in terms of its implicit class. This function is
     *  used to provide an efficient way to read the class linking offsets for
     *  processes that does not need to read the entire representation.
     *
     *  The provided offset value indicates where to start the representation
     *  reading in the provided stream. The provided stream has to be an already
     *  opened stream in read-binary mode.
     *
     *  \param le_mono   Mono-vertex class structure
     *  \param le_offset Class reading offset, in bytes
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_mono_io_read_fast( le_mono_t * const le_mono, le_size_t const le_offset, le_file_t const le_stream );

    /*! \brief i/o methods
     *
     *  This function allows to write the content of the provided mono-vertex
     *  class structure in the provided stream to create its tree structure
     *  storage representation in the specified stream.
     *
     *  Before to write the class in the stream, the function computes the mean
     *  color of the class by reading the content of the accumulation array and
     *  the state of the counter.
     *
     *  If a null offset is provided, the function assumes that the class has to
     *  be written at the current position of the provided stream. The provided
     *  stream has to be an already opened stream in binary write mode.
     *
     *  \param le_mono   Mono-vertex class structure
     *  \param le_offset Class writing offset, in bytes
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_mono_io_write( le_mono_t * const le_mono, le_size_t const le_offset, le_file_t const le_stream );

    /*! \brief i/o methods (detached)
     *
     *  Note :
     *
     *  Detached methods are specific functions associated to a structure but
     *  without operating directly on it. Such function are used in optimisation
     *  strategies.
     *
     *  This function allows the reads a single offset from a mono-vertex class
     *  representation in a tree storage structure. The function invokes the
     *  specialised implicit class function to read the desired offset.
     *
     *  The provided offset value indicates where to start the representation
     *  reading in the provided stream. The provided stream has to be an already
     *  opened stream in binary read mode.
     *
     *  \param le_offset Class reading offset, in bytes
     *  \param le_index  Class index
     *  \param le_stream Stream descriptor
     *
     *  \return Returns the class desired offset on success, _LE_OFFS_NULL
     *  otherwise
     */

    le_size_t le_mono_io_offset( le_size_t const le_offset, le_size_t const le_index, le_file_t const le_stream );

    /*! \brief i/o methods (detached)
     *
     *  Note :
     *
     *  Detached methods are specific functions associated to a structure but
     *  without operating directly on it. Such function are used in optimisation
     *  strategies.
     *
     *  This function allows to read the color information from a mono-vertex
     *  class representation in a tree storage structure. The function only
     *  reads the color information, discarding the remaining information.
     *
     *  The provided offset value indicates where to start the representation
     *  reading in the provided stream. The provided stream has to be an already
     *  opened stream in binary read mode.
     *
     *  \param le_offset Class reading offset, in bytes
     *  \param le_data   Pointer to the color array, in 8-bits RGB
     *  \param le_stream Stream descriptor
     *
     *  \return Returns LE_ERROR_SUCCESS on success, an error code otherwise
     */

    le_enum_t le_mono_io_data( le_size_t const le_offset, le_byte_t * const le_data, le_file_t const le_stream );

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

