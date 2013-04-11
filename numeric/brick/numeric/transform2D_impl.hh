/**
***************************************************************************
* @file transform2D_impl.hh
*
* Header file providing implementations for inline and template
* functions declared in transform2D.hh.
*
* Copyright (C) 2001-2013 David LaRose, dlr@cs.cmu.edu
* See accompanying file, LICENSE.TXT, for details.
*
***************************************************************************
**/

#ifndef BRICK_NUMERIC_TRANSFORM2D_IMPL_HH
#define BRICK_NUMERIC_TRANSFORM2D_IMPL_HH

// This file is included by transform2D.hh, and should not be directly
// included by user code, so no need to include transform2D.hh here.
// 
// #include <brick/numeric/transform2D.hh>

namespace brick {

  namespace numeric {
    
    // Default constructor.  Initializes to identity.
    template <class Type>
    inline
    Transform2D<Type>::
    Transform2D()
      : m_00(1.0), m_01(0.0), m_02(0.0),
        m_10(0.0), m_11(1.0), m_12(0.0),
        m_20(0.0), m_21(0.0), m_22(1.0)
    {
      // Empty.
    }


    // Build a Transform2D instance by explicitly setting element values
    // as if setting the elements of a 3x3 transformation matrix.
    template <class Type>
    inline
    Transform2D<Type>::
    Transform2D(Type const& a00, Type const& a01, Type const& a02,
                Type const& a10, Type const& a11, Type const& a12,
                Type const& a20, Type const& a21, Type const& a22,
                bool doNormalize)
      : m_00(a00), m_01(a01), m_02(a02),
        m_10(a10), m_11(a11), m_12(a12),
        m_20(a20), m_21(a21), m_22(a22)
    {
      if(doNormalize) {
        this->normalize();
      }
    }

    
    // Build a Transform2D from a homogeneous 3x3 matrix.
    template <class Type>
    Transform2D<Type>::
    Transform2D(const Array2D<Type>& source, bool doNormalize)
    {
      if((source.rows() != 3) || (source.columns() != 3)) {
        std::ostringstream message;
        message << "Can't create a Transform2D from a " << source.rows()
                << " x " << source.columns() << "Array2D instance.";
        BRICK_THROW(common::ValueException, "Transform2D::Transform2D()",
                  message.str().c_str());
      }
      m_00 = source(0); m_01 = source(1); m_02 = source(2);
      m_10 = source(3); m_11 = source(4); m_12 = source(5);
      m_20 = source(6); m_21 = source(7); m_22 = source(8);
      if(doNormalize) {
        this->normalize();
      }
    }


    // The copy constructor simply duplicates its argument.
    template <class Type>
    inline
    Transform2D<Type>::
    Transform2D(Transform2D<Type> const& src)
      : m_00(src.m_00), m_01(src.m_01), m_02(src.m_02),
        m_10(src.m_10), m_11(src.m_11), m_12(src.m_12),
        m_20(src.m_20), m_21(src.m_21), m_22(src.m_22)
    {
      // Empty.
    }

    
    // This member function returns a functor which makes it easier to
    // transform arrays of points using algorithms such as
    // std::transform().
    template <class Type>
    Transform2DFunctor<Type>
    Transform2D<Type>::
    getFunctor() const
    {
      return Transform2DFunctor<Type>(*this);
    }    
  
  
    // This operator returns one element from the matrix
    // representation of the coordinate transform by value.
    // The case statements should optimize away, since row and column
    // are both known at compile time.
    template <class Type>
    template <size_t row, size_t column>
    Type const&
    Transform2D<Type>::
    getValue() const
    {
      // // Avoid ugly duplication of code using ugly const_cast.
      // return const_cast<Transform2D*>(this)->value<row, column>();
      switch(row) {
      case 0:
        switch(column) {
        case 0: return m_00; break;
        case 1: return m_01; break;
        case 2: return m_02; break;
        default: break;
        }
        break;
      case 1:
        switch(column) {
        case 0: return m_10; break;
        case 1: return m_11; break;
        case 2: return m_12; break;
        default: break;
        }
        break;
      case 2:
        switch(column) {
        case 0: return m_20; break;
        case 1: return m_21; break;
        case 2: return m_22; break;
        default: break;
        }
        break;
      default:
        break;
      }
      std::ostringstream message;
      message << "Index (" << row << ", " << column << ") out of bounds.";
      BRICK_THROW(common::IndexException, "Transform2D::value<>()",
                  message.str().c_str());
      return m_22; // Dummy return to keep the compiler happy.
    }

    
    // This member function returns the inverse of *this.
    template <class Type>
    Transform2D<Type>
    Transform2D<Type>::
    invert() const
    {
      // We use the cofactor method for now, since it's easier to code
      // than Gauss-Jordan elimination.  We suspect that it's less
      // efficient, however.
    
      // Notation for determinant values is detRRCC, where the
      // Rs indicate the involved rows, from top to bottom, and the Cs
      // indicate the involved columns, from left to right.

      Type det0101 = m_00 * m_11 - m_01 * m_10;
      Type det0102 = m_00 * m_12 - m_02 * m_10;
      Type det0112 = m_01 * m_12 - m_02 * m_11;

      Type det0201 = m_00 * m_21 - m_01 * m_20;
      Type det0202 = m_00 * m_22 - m_02 * m_20;
      Type det0212 = m_01 * m_22 - m_02 * m_21;

      Type det1201 = m_10 * m_21 - m_11 * m_20;
      Type det1202 = m_10 * m_22 - m_12 * m_20;
      Type det1212 = m_11 * m_22 - m_12 * m_21;

      Type det012012 =  m_00 * det1212 - m_01 * det1202 + m_02 * det1201;
    
      // Note that in general, roundoff error will make us pass this
      // test, even for singular matrices.
      if(det012012 == 0.0) {
        BRICK_THROW(common::ValueException, "Transform2D::invert()",
                  "Transform is not invertible.");
      }
    
      return Transform2D(
        det1212 / det012012, -det0212 / det012012, det0112 / det012012,
        -det1202 / det012012, det0202 / det012012, -det0102 / det012012,
        det1201 / det012012, -det0201 / det012012, det0101 / det012012);
    }

  
    // Change the Transform2D value by explicitly setting element values
    // as if setting the elements of a 3x3 transformation matrix:
    //    [[a00, a01, a02],
    //     [a10, a11, a12],
    //     [a20, a21, a22]]
    template <class Type>
    void
    Transform2D<Type>::
    setTransform(Type const& a00, Type const& a01, Type const& a02,
                 Type const& a10, Type const& a11, Type const& a12,
                 Type const& a20, Type const& a21, Type const& a22,
                 bool doNormalize)
    {
      m_00 = a00; m_01 = a01; m_02 = a02;
      m_10 = a10; m_11 = a11; m_12 = a12;
      m_20 = a20; m_21 = a21; m_22 = a22;
      if(doNormalize) {
        this->normalize();
      }
    }


    // This member function sets one element from the matrix
    // representation of the coordinate transform to the specified
    // value.
    template <class Type>
    void
    Transform2D<Type>::
    setValue(size_t row, size_t column, Type const& val)
    {
      switch(row) {
      case 0:
        switch(column) {
        case 0: m_00 = val; return; break;
        case 1: m_01 = val; return; break;
        case 2: m_02 = val; return; break;
        default: break;
        }
        break;
      case 1:
        switch(column) {
        case 0: m_10 = val; return; break;
        case 1: m_11 = val; return; break;
        case 2: m_12 = val; return; break;
        default: break;
        }
        break;
      case 2:
        switch(column) {
        case 0: m_20 = val; return; break;
        case 1: m_21 = val; return; break;
        case 2: m_22 = val; return; break;
        default: break;
        }
        break;
      default:
        break;
      }
      std::ostringstream message;
      message << "Indices (" << row << ", " << column << ") are out of bounds.";
      BRICK_THROW(common::IndexException, "Transform2D::operator()(size_t, size_t)",
                message.str().c_str());
    }


    // This operator sets one element of the matrix representation of
    // the coordinate transform.  The case statements should optimize
    // away, since row and column are both known at compile time.
    template <class Type>
    template <size_t row, size_t column>
    void
    Transform2D<Type>::
    setValue(Type const& val)
    {
      switch(row) {
      case 0:
        switch(column) {
        case 0: m_00 = val; return; break;
        case 1: m_01 = val; return; break;
        case 2: m_02 = val; return; break;
        default: break;
        }
        break;
      case 1:
        switch(column) {
        case 0: m_10 = val; return; break;
        case 1: m_11 = val; return; break;
        case 2: m_12 = val; return; break;
        default: break;
        }
        break;
      case 2:
        switch(column) {
        case 0: m_20 = val; return; break;
        case 1: m_21 = val; return; break;
        case 2: m_22 = val; return; break;
        default: break;
        }
        break;
      default:
        break;
      }
      std::ostringstream message;
      message << "Indices (" << row << ", " << column << ") are out of bounds.";
      BRICK_THROW(common::IndexException,
                  "Transform2D::setValue<size_t, size_t>(Type)",
                  message.str().c_str());
    }

    
    // This operator returns one element from the matrix
    // representation of the coordinate transform by value.
    template <class Type>
    Type const&
    Transform2D<Type>::
    operator()(size_t row, size_t column) const
    {
      // // Avoid ugly duplication of code using ugly const_cast.
      // return const_cast<Transform2D*>(this)->operator()(row, column);
      switch(row) {
      case 0:
        switch(column) {
        case 0: return m_00; break;
        case 1: return m_01; break;
        case 2: return m_02; break;
        default: break;
        }
        break;
      case 1:
        switch(column) {
        case 0: return m_10; break;
        case 1: return m_11; break;
        case 2: return m_12; break;
        default: break;
        }
        break;
      case 2:
        switch(column) {
        case 0: return this->value<2, 0>(); break;
        case 1: return m_21; break;
        case 2: return m_22; break;
        default: break;
        }
        break;
      default:
        break;
      }
      std::ostringstream message;
      message << "Index (" << row << ", " << column << ") out of bounds.";
      BRICK_THROW(common::IndexException, "Transform2D::value()",
                 message.str().c_str());
      return m_22; // Dummy return to keep the compiler happy.
    }

    
    // This operator takes a point and applies the coordinate
    // transform, returning the result.
    template <class Type>
    Vector2D<Type>
    Transform2D<Type>::
    operator*(Vector2D<Type> const& vector0) const
    {
      return Vector2D<Type>(
        m_00 * vector0.x() + m_01 * vector0.y() + m_02,
        m_10 * vector0.x() + m_11 * vector0.y() + m_12,
        m_20 * vector0.x() + m_21 * vector0.y() + m_22);
    }

    
    // The assignment operator simply duplicates its argument.
    template <class Type>
    Transform2D<Type>&
    Transform2D<Type>::
    operator=(Transform2D<Type> const& source)
    {
      if(&source != this) {
        m_00 = source.m_00; m_01 = source.m_01; m_02 = source.m_02;
        m_10 = source.m_10; m_11 = source.m_11; m_12 = source.m_12;
        m_20 = source.m_20; m_21 = source.m_21; m_22 = source.m_22;
      }
      return *this;
    }

    
    template <class Type>
    void
    Transform2D<Type>::
    normalize()
    {
      if(m_22 == 0.0) {
        BRICK_THROW(common::ValueException, "Trahnsform2D::normalize(Type)",
                   "Invalid normalization constant. "
                   "The bottom right element of a homogeneous transformation "
                   "cannot be equal to 0.0.");
      }
      if(m_22 != 1.0) {
        m_00 /= m_22;
        m_01 /= m_22;
        m_02 /= m_22;
        m_10 /= m_22;
        m_11 /= m_22;
        m_12 /= m_22;
        m_20 /= m_22;
        m_21 /= m_22;
        m_22 /= m_22;
      }
    }

    
    /* ============== Non-member functions  ============== */
  
    // This operator composes two Transform2D instances.  The resulting
    // transform satisfies the equation:
    //   (transform0 * transform1) * v0 = transform0 * (transform1 * v0),
    // where v0 is a Vector2D instance.
    template <class Type>
    Transform2D<Type>
    operator*(Transform2D<Type> const& transform0,
              Transform2D<Type> const& transform1)
    {
      // We'd rather use the templated getValue() member here, as it's
      // faster than operator()(), but for some reason the compiler is
      // choking on it, so we default to what works.  We repair most
      // of the damage by specializing for double and float below.
#if 0
      Type a00 = (transform0.value<0, 0>() * transform1.value<0, 0>()
                  + transform0.value<0, 1>() * transform1.value<1, 0>()
                  + transform0.value<0, 2>() * transform1.value<2, 0>());
      Type a01 = (transform0.value<0, 0>() * transform1.value<0, 1>()
                    + transform0.value<0, 1>() * transform1.value<1, 1>()
                    + transform0.value<0, 2>() * transform1.value<2, 1>());
      Type a02 = (transform0.value<0, 0>() * transform1.value<0, 2>()
                    + transform0.value<0, 1>() * transform1.value<1, 2>()
                    + transform0.value<0, 2>() * transform1.value<2, 2>());
      Type a10 = (transform0.value<1, 0>() * transform1.value<0, 0>()
                    + transform0.value<1, 1>() * transform1.value<1, 0>()
                    + transform0.value<1, 2>() * transform1.value<2, 0>());
      Type a11 = (transform0.value<1, 0>() * transform1.value<0, 1>()
                    + transform0.value<1, 1>() * transform1.value<1, 1>()
                    + transform0.value<1, 2>() * transform1.value<2, 1>());
      Type a12 = (transform0.value<1, 0>() * transform1.value<0, 2>()
                    + transform0.value<1, 1>() * transform1.value<1, 2>()
                    + transform0.value<1, 2>() * transform1.value<2, 2>());
      Type a20 = (transform0.value<2, 0>() * transform1.value<0, 0>()
                    + transform0.value<2, 1>() * transform1.value<1, 0>()
                    + transform0.value<2, 2>() * transform1.value<2, 0>());
      Type a21 = (transform0.value<2, 0>() * transform1.value<0, 1>()
                    + transform0.value<2, 1>() * transform1.value<1, 1>()
                    + transform0.value<2, 2>() * transform1.value<2, 1>());
      Type a22 = (transform0.value<2, 0>() * transform1.value<0, 2>()
                    + transform0.value<2, 1>() * transform1.value<1, 2>()
                    + transform0.value<2, 2>() * transform1.value<2, 2>());
#else /* #if 0 */
      Type a00 = (transform0(0, 0) * transform1(0, 0)
                  + transform0(0, 1) * transform1(1, 0)
                  + transform0(0, 2) * transform1(2, 0));
      Type a01 = (transform0(0, 0) * transform1(0, 1)
                  + transform0(0, 1) * transform1(1, 1)
                  + transform0(0, 2) * transform1(2, 1));
      Type a02 = (transform0(0, 0) * transform1(0, 2)
                  + transform0(0, 1) * transform1(1, 2)
                  + transform0(0, 2) * transform1(2, 2));
      Type a10 = (transform0(1, 0) * transform1(0, 0)
                  + transform0(1, 1) * transform1(1, 0)
                  + transform0(1, 2) * transform1(2, 0));
      Type a11 = (transform0(1, 0) * transform1(0, 1)
                  + transform0(1, 1) * transform1(1, 1)
                  + transform0(1, 2) * transform1(2, 1));
      Type a12 = (transform0(1, 0) * transform1(0, 2)
                  + transform0(1, 1) * transform1(1, 2)
                  + transform0(1, 2) * transform1(2, 2));
      Type a20 = (transform0(2, 0) * transform1(0, 0)
                  + transform0(2, 1) * transform1(1, 0)
                  + transform0(2, 2) * transform1(2, 0));
      Type a21 = (transform0(2, 0) * transform1(0, 1)
                  + transform0(2, 1) * transform1(1, 1)
                  + transform0(2, 2) * transform1(2, 1));
      Type a22 = (transform0(2, 0) * transform1(0, 2)
                  + transform0(2, 1) * transform1(1, 2)
                  + transform0(2, 2) * transform1(2, 2));
#endif /* #if 0 */

      return Transform2D<Type>(a00, a01, a02,
                         a10, a11, a12,
                         a20, a21, a22);
    }

    
    template <class Type>
    std::ostream&
    operator<<(std::ostream& stream, const Transform2D<Type>& transform0)
    {
      stream << "Transform2D("
             << transform0(0, 0) << ", "
             << transform0(0, 1) << ", "
             << transform0(0, 2) << ", "
             << transform0(1, 0) << ", "
             << transform0(1, 1) << ", "
             << transform0(1, 2) << ", "
             << transform0(2, 0) << ", "
             << transform0(2, 1) << ", "
             << transform0(2, 2) << ")";
      return stream;
    }

  
    template <class Type>
    std::istream&
    operator>>(std::istream& stream, Transform2D<Type>& transform0)
    {
      // If stream is in a bad state, we can't read from it.
      if (!stream){
        return stream;
      }
    
      // It's a lot easier to use a try block than to be constantly
      // testing whether the IO has succeeded, so we tell stream to
      // complain if anything goes wrong.
      std::ios_base::iostate oldExceptionState = stream.exceptions();
      stream.exceptions(
        std::ios_base::badbit | std::ios_base::failbit | std::ios_base::eofbit);

      // Now on with the show.
      try{
        common::Expect::FormatFlag flags = common::Expect::SkipWhitespace;

        // Skip any preceding whitespace.
        stream >> common::Expect("", flags);
      
        // Read the "Transform2D(" part.
        stream >> common::Expect("Transform2D(", flags);

        // Read all the data except the last element.
        std::vector<Type> inputValues(9);
        for(size_t index = 0; index < (inputValues.size() - 1); ++index) {
          // Read the value.
          stream >> inputValues[index];

          // Read punctuation before the next value.
          stream >> common::Expect(",", flags);
        }

        // Read the final value.
        stream >> inputValues[inputValues.size() - 1];

        // Read the closing parenthesis.
        stream >> common::Expect(")", flags);

        // And update the transform.
        transform0.setTransform(
          inputValues[0], inputValues[1], inputValues[2],
          inputValues[3], inputValues[4], inputValues[5],
          inputValues[6], inputValues[7], inputValues[8]);

      } catch(std::ios_base::failure) {
        // Empty
      }
      stream.exceptions(oldExceptionState);
      return stream;
    }
    
  } // namespace numeric

} // namespace brick

#endif /* #ifndef BRICK_NUMERIC_TRANSFORM2D_IMPL_HH */
