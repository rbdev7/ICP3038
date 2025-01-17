#include <sstream>
#include <stdexcept>      // std::out_of_range
#include <cmath>

#ifdef HAS_LIBJPEG
#include <jerror.h>
#include <jpeglib.h>
#endif

#include "Image.h"


//------------------------------------------------------
Image operator*(float aValue, const Image& anInputImage)
//------------------------------------------------------
{
    Image temp = anInputImage;
    
    float* p_data = temp.getPixelPointer();
    size_t number_of_pixels = temp.getWidth() * temp.getHeight();
    
    for (size_t i = 0; i < number_of_pixels; ++i)
    {
        *p_data++ *= aValue;
    }
    
    return temp;
}


//------------------------------------------------------
Image operator+(float aValue, const Image& anInputImage)
//------------------------------------------------------
{
    Image temp = anInputImage;
    
    float* p_data = temp.getPixelPointer();
    size_t number_of_pixels = temp.getWidth() * temp.getHeight();
    
    for (size_t i = 0; i < number_of_pixels; ++i)
    {
        *p_data++ += aValue;
    }
    
    return temp;
}


//--------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& anOutputStream, const Image& anImage)
//--------------------------------------------------------------------------
{
    // Output all the rows
    for (unsigned row = 0; row < anImage.getHeight(); ++row)
    {
        // Output all the columns
        for (unsigned col = 0; col < anImage.getWidth(); ++col)
        {
            // Output the corresponding pixel
            anOutputStream << anImage(col, row);

            // Add a space charater if needed
            if (col < anImage.getWidth() - 1)
            {
                anOutputStream << " ";
            }
        }

        // Add a end of line if needed
        if (row < anImage.getHeight() - 1)
        {
            anOutputStream << std::endl;
        }
    }

    // Return the stream
    return anOutputStream;
}


//---------------------------
Image::Image():
//---------------------------
    m_width(0),
    m_height(0),
    m_min_pixel_value(0),
    m_max_pixel_value(0),
    m_average_pixel_value(0),
    m_stddev_pixel_value(0),
    m_stats_up_to_date(true)
//---------------------------
{}


//-------------------------------------------------------
Image::Image(const Image& anImage):
//-------------------------------------------------------
    m_pixel_data(anImage.m_pixel_data),
    m_width(anImage.m_width),
    m_height(anImage.m_height),
    m_min_pixel_value(anImage.m_min_pixel_value),
    m_max_pixel_value(anImage.m_max_pixel_value),
    m_average_pixel_value(anImage.m_average_pixel_value),
    m_stddev_pixel_value(anImage.m_stddev_pixel_value),
    m_stats_up_to_date(anImage.m_stats_up_to_date)
//-------------------------------------------------------
{}


//----------------------------------------------------------------
Image::Image(const float* anImage, size_t aWidth, size_t aHeight):
//----------------------------------------------------------------
    m_pixel_data(anImage, anImage + aWidth * aHeight),
    m_width(aWidth),
    m_height(aHeight),
    m_min_pixel_value(0),
    m_max_pixel_value(0),
    m_average_pixel_value(0),
    m_stddev_pixel_value(0),
    m_stats_up_to_date(false)
//----------------------------------------------------------------
{}


//-----------------------------------------------------------------------------
Image::Image(const std::vector<float>& anImage, size_t aWidth, size_t aHeight):
//-----------------------------------------------------------------------------
    m_pixel_data(anImage),
    m_width(aWidth),
    m_height(aHeight),
    m_min_pixel_value(0),
    m_max_pixel_value(0),
    m_average_pixel_value(0),
    m_stddev_pixel_value(0),
    m_stats_up_to_date(false)
//-----------------------------------------------------------------------------
{}


//-----------------------------------------------------------
Image::Image(float aConstant, size_t aWidth, size_t aHeight):
//-----------------------------------------------------------
    m_pixel_data(aWidth * aHeight, aConstant),
    m_width(aWidth),
    m_height(aHeight),
    m_min_pixel_value(aConstant),
    m_max_pixel_value(aConstant),
    m_average_pixel_value(aConstant),
    m_stddev_pixel_value(0),
    m_stats_up_to_date(true)
//-----------------------------------------------------------
{}


//----------------------------------
Image::Image(const char* aFilename):
//----------------------------------
    m_width(0),
    m_height(0),
    m_min_pixel_value(0),
    m_max_pixel_value(0),
    m_average_pixel_value(0),
    m_stddev_pixel_value(0),
    m_stats_up_to_date(true)
//---------------------------
{
    load(aFilename);
}


//-----------------------------------------
Image::Image(const std::string& aFilename):
//-----------------------------------------
    m_width(0),
    m_height(0),
    m_min_pixel_value(0),
    m_max_pixel_value(0),
    m_average_pixel_value(0),
    m_stddev_pixel_value(0),
    m_stats_up_to_date(true)
//-----------------------------------------
{
    load(aFilename);
}


//------------------------------------------------
Image& Image::operator=(const Image& anInputImage)
//------------------------------------------------
{
    m_pixel_data = anInputImage.m_pixel_data;
    m_width = anInputImage.m_width;
    m_height = anInputImage.m_height;
    m_min_pixel_value = anInputImage.m_min_pixel_value;
    m_max_pixel_value = anInputImage.m_max_pixel_value;
    m_average_pixel_value = anInputImage.m_average_pixel_value;
    m_stddev_pixel_value = anInputImage.m_stddev_pixel_value;
    m_stats_up_to_date = anInputImage.m_stats_up_to_date;

    return *this;
}


//--------------------------------------------
Image& Image::operator=(const char* aFileName)
//--------------------------------------------
{
    load(aFileName);
    return *this;
}


//---------------------------------------------------
Image& Image::operator=(const std::string& aFileName)
//---------------------------------------------------
{
    load(aFileName);
    return *this;
}


//-------------------------------------
void Image::load(const char* aFilename)
//-------------------------------------
{
#ifdef HAS_LIBJPEG
    // Allocate and initialize a JPEG decompression object
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // Specify the source of the compressed data (eg, a file)
    FILE* p_input_file(fopen(aFilename, "rb"));
    if (!p_input_file)
    {
        // Format a nice error message
        std::stringstream error_message;
        error_message << "ERROR:" << std::endl;
        error_message << "\tin File:" << __FILE__ << std::endl;
        error_message << "\tin Function:" << __FUNCTION__ << std::endl;
        error_message << "\tat Line:" << __LINE__ << std::endl;
        error_message << "\tMESSAGE: Can't open " << aFilename << std::endl;
        throw std::runtime_error(error_message.str());
    }
    jpeg_stdio_src(&cinfo, p_input_file);

    // Call jpeg_read_header() to obtain image info
    jpeg_read_header(&cinfo, TRUE);

    // Set parameters for decompression
    // Nothing to be done if the defaults are appropriate

    // Start decompression
    jpeg_start_decompress(&cinfo);

    // Save the size of the image
    m_width = cinfo.output_width;
    m_height = cinfo.output_height;
    m_pixel_data.resize(m_width * m_height);

    // Decompress data
    JSAMPROW row_pointer[1];        // pointer to a single row
    int row_stride;                 // physical row width in buffer

    if (cinfo.out_color_space == JCS_RGB)
    {
        row_stride = m_width * 3;   // JSAMPLEs per row in image_buffer
    }
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
    {
        row_stride = m_width;   // JSAMPLEs per row in image_buffer
    }
    // Unknown colour space
    else
    {
        // Format a nice error message
        std::stringstream error_message;
        error_message << "ERROR:" << std::endl;
        error_message << "\tin File:" << __FILE__ << std::endl;
        error_message << "\tin Function:" << __FUNCTION__ << std::endl;
        error_message << "\tat Line:" << __LINE__ << std::endl;
        error_message << "\tMESSAGE: Unknown colour space" << std::endl;
        throw std::runtime_error(error_message.str());
    }

    // Create temporary buffer
    unsigned char* p_rgb_image(new unsigned char[m_width * m_height * 3]);

    // Decompress the data
    while (cinfo.output_scanline < cinfo.output_height)
    {
        row_pointer[0] = & p_rgb_image[cinfo.output_scanline * row_stride];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    // Finish decompression
    jpeg_finish_decompress(&cinfo);

    // Release the JPEG decompression object
    jpeg_destroy_decompress(&cinfo);

    // Convert the data
    if (cinfo.out_color_space == JCS_RGB)
    {
        double pixel_value;

        for (size_t i(0); i < m_width * m_height; ++i)
        {
            // Compute the relative luminance from RGB data
            // using Photometric/digital ITU-R
            pixel_value  = 0.2126 * p_rgb_image[i * 3    ];
            pixel_value += 0.7152 * p_rgb_image[i * 3 + 1];
            pixel_value += 0.0722 * p_rgb_image[i * 3 + 2];

            m_pixel_data[i] = pixel_value;
        }
    }
    // Copy the data
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
    {
        for (unsigned int i(0); i < m_width * m_height; ++i)
        {
            m_pixel_data[i] = p_rgb_image[i];
        }
    }

    // Release memory
    delete [] p_rgb_image;

    // The statistics is not up-to-date
    m_stats_up_to_date = false;
#else
    // Format a nice error message
    std::stringstream error_message;
    error_message << "ERROR:" << std::endl;
    error_message << "\tin File:" << __FILE__ << std::endl;
    error_message << "\tin Function:" << __FUNCTION__ << std::endl;
    error_message << "\tat Line:" << __LINE__ << std::endl;
    error_message << "\tMESSAGE: LibJPEG not supported" << std::endl;
    throw std::runtime_error(error_message.str());
#endif
}


//--------------------------------------------
void Image::load(const std::string& aFilename)
//--------------------------------------------
{
    load(aFilename.c_str());
}


//-----------------------------------------
void Image::saveJPEG(const char* aFilename)
//-----------------------------------------
{
#ifdef HAS_LIBJPEG
    // Allocate and initialize a JPEG compression object
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // Specify the destination for the compressed data (eg, a file)
    FILE* p_output_file(fopen(aFilename, "wb"));
    if (!p_output_file)
    {
        // Format a nice error message
        std::stringstream error_message;
        error_message << "ERROR:" << std::endl;
        error_message << "\tin File:" << __FILE__ << std::endl;
        error_message << "\tin Function:" << __FUNCTION__ << std::endl;
        error_message << "\tat Line:" << __LINE__ << std::endl;
        error_message << "\tMESSAGE: Can't open " << aFilename << std::endl;
        throw std::runtime_error(error_message.str());
    }
    jpeg_stdio_dest(&cinfo, p_output_file);

    // Set parameters for compression, including image size & colorspace
    cinfo.image_width  = m_width;   // image width in pixels
    cinfo.image_height = m_height;  // image height in pixels
    cinfo.input_components = 3;     // number of color components per pixel
    cinfo.in_color_space = JCS_RGB; // colorspace of input image
    jpeg_set_defaults(&cinfo);

    // Create temporary buffer
    unsigned char* p_rgb_image(new unsigned char[m_width * m_height *3]);

    // Convert the data
    double input_pixel_value;
    unsigned char output_pixel_value;

    for (unsigned int i(0); i < m_width * m_height; ++i)
    {
        input_pixel_value = m_pixel_data[i];

        if (input_pixel_value <= 0.0f)
        {
            output_pixel_value = 0;
        }
        else if (input_pixel_value >= 255.0f)
        {
            output_pixel_value = 255;
        }
        else
        {
            output_pixel_value = input_pixel_value;
        }

        p_rgb_image[i * 3    ] = output_pixel_value;
        p_rgb_image[i * 3 + 1] = output_pixel_value;
        p_rgb_image[i * 3 + 2] = output_pixel_value;
    }


    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Compress data
    JSAMPROW row_pointer[1];        // pointer to a single row
    int row_stride;                 // physical row width in buffer

    row_stride = m_width * 3;   // JSAMPLEs per row in image_buffer

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = & p_rgb_image[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Finish compression
    jpeg_finish_compress(&cinfo);

    // Release the JPEG compression object
    jpeg_destroy_compress(&cinfo);

    // Release memory
    delete [] p_rgb_image;
#else
    // Format a nice error message
    std::stringstream error_message;
    error_message << "ERROR:" << std::endl;
    error_message << "\tin File:" << __FILE__ << std::endl;
    error_message << "\tin Function:" << __FUNCTION__ << std::endl;
    error_message << "\tat Line:" << __LINE__ << std::endl;
    error_message << "\tMESSAGE: LibJPEG not supported" << std::endl;
    throw std::runtime_error(error_message.str());
#endif
}


//------------------------------------------------
void Image::saveJPEG(const std::string& aFilename)
//------------------------------------------------
{
    saveJPEG(aFilename.c_str());
}


//----------------------------------------------------------
const float& Image::operator()(size_t col, size_t row) const
//----------------------------------------------------------
{
    // Check if the coordinates are valid, if not throw an error
    if (col < 0 || col >= m_width || row < 0 || row >= m_height)
    {
        // Format a nice error message
        std::stringstream error_message;
        error_message << "ERROR:" << std::endl;
        error_message << "\tin File:" << __FILE__ << std::endl;
        error_message << "\tin Function:" << __FUNCTION__ << std::endl;
        error_message << "\tat Line:" << __LINE__ << std::endl;
        error_message << "\tMESSAGE: Pixel(" << col << ", " << row << ") does not exist. The image size is: " << m_width << "x" << m_height << std::endl;

        // Throw an exception
        throw std::out_of_range(error_message.str());
    }

    return m_pixel_data[row * m_width + col];
}


//----------------------------------------------
float& Image::operator()(size_t col, size_t row)
//----------------------------------------------
{
    // Check if the coordinates are valid, if not throw an error
    if (col < 0 || col >= m_width || row < 0 || row >= m_height)
    {
        // Format a nice error message
        std::stringstream error_message;
        error_message << "ERROR:" << std::endl;
        error_message << "\tin File:" << __FILE__ << std::endl;
        error_message << "\tin Function:" << __FUNCTION__ << std::endl;
        error_message << "\tat Line:" << __LINE__ << std::endl;
        error_message << "\tMESSAGE: Pixel(" << col << ", " << row << ") does not exist. The image size is: " << m_width << "x" << m_height << std::endl;

        // Throw an exception
        throw std::out_of_range(error_message.str());
    }

    // To be on the safe side, turn the flag off
    m_stats_up_to_date = false;

    return m_pixel_data[row * m_width + col];
}


//----------------------------
size_t Image::getWidth() const
//----------------------------
{
    return m_width;
}


//-----------------------------
size_t Image::getHeight() const
//-----------------------------
{
    return m_height;
}


//-----------------------------------------
const float* Image::getPixelPointer() const
//-----------------------------------------
{
    // There are pixels
    if (m_pixel_data.size() && m_width && m_height)
        return &m_pixel_data[0];
    // There is no pixel
    else
        return 0;
}

//-----------------------------
float* Image::getPixelPointer()
//-----------------------------
{
    // To be on the safe side, turn the flag off
    m_stats_up_to_date = false;

    // There are pixels
    if (m_pixel_data.size() && m_width && m_height)
        return &m_pixel_data[0];
    // There is no pixel
    else
        return 0;
}


//----------------------------------------
Image Image::operator+(float aValue) const
//----------------------------------------
{
    Image temp = *this;
    
    for (size_t i = 0; i < m_width * m_height; ++i)
    {
        temp.m_pixel_data[i] += aValue;
    }
    
    return temp;
}


//----------------------------------------
Image Image::operator-(float aValue) const
//----------------------------------------
{
    Image temp = *this;
    
    for (size_t i = 0; i < m_width * m_height; ++i)
    {
        temp.m_pixel_data[i] -= aValue;
    }
    
    return temp;
}


//----------------------------------------
Image Image::operator*(float aValue) const
//----------------------------------------
{
    Image temp = *this;
    
    for (size_t i = 0; i < m_width * m_height; ++i)
    {
        temp.m_pixel_data[i] *= aValue;
    }
    
    return temp;
}


//----------------------------------------
Image Image::operator/(float aValue) const
//----------------------------------------
{
    Image temp = *this;
    
    for (size_t i = 0; i < m_width * m_height; ++i)
    {
        temp.m_pixel_data[i] /= aValue;
    }
    
    return temp;
}


//------------------------------------
Image& Image::operator+=(float aValue)
//------------------------------------
{
    *this = *this + aValue;
    return *this;
}


//------------------------------------
Image& Image::operator-=(float aValue)
//------------------------------------
{
    *this = *this - aValue;
    return *this;
}


//------------------------------------
Image& Image::operator*=(float aValue)
//------------------------------------
{
    *this = *this * aValue;
    return *this;
}


//------------------------------------
Image& Image::operator/=(float aValue)
//------------------------------------
{
    *this = *this / aValue;
    return *this;
}


//----------------------
Image Image::normalise()
//----------------------
{
    return (*this - getMinValue()) / (getMaxValue() - getMinValue());
}

//Image Image::operator!() const;


//------------------------
float Image::getMinValue()
//------------------------
{
    if (!m_stats_up_to_date) updateStats();
    
    return m_min_pixel_value;
}


//------------------------
float Image::getMaxValue()
//------------------------
{
    if (!m_stats_up_to_date) updateStats();
    
    return m_max_pixel_value;
}


//-----------------------
void Image::updateStats()
//-----------------------
{
    // Need to udate the stats
    if (!m_stats_up_to_date && m_width * m_height)
    {
        m_min_pixel_value = m_pixel_data[0];
        m_max_pixel_value = m_pixel_data[0];
        m_average_pixel_value = m_pixel_data[0];
        
        for (size_t i = 1; i < m_width * m_height; ++i)
        {
            if (m_min_pixel_value > m_pixel_data[i]) m_min_pixel_value = m_pixel_data[i];
            if (m_max_pixel_value < m_pixel_data[i]) m_max_pixel_value = m_pixel_data[i];
            
            m_average_pixel_value += m_pixel_data[i];
        }
        m_average_pixel_value /= m_width * m_height;
        
        m_stddev_pixel_value = 0;
        for (size_t i = 0; i < m_width * m_height; ++i)
        {
            m_stddev_pixel_value += (m_pixel_data[i] - m_average_pixel_value) * (m_pixel_data[i] - m_average_pixel_value);
        }
        m_stddev_pixel_value /= m_width * m_height;
        m_stddev_pixel_value = sqrt(m_stddev_pixel_value);
        
        m_stats_up_to_date = true;
    }
}

