#ifndef __Image_h
#define __Image_h

#include <vector>
#include <string>
#include <iostream>

class Image;
std::ostream& operator<<(std::ostream& anOutputStream, const Image& anImage);

class Image
{
public:
    //--------------------------------------------------------------------------
    /// Default constructor: Create an empty image
    //--------------------------------------------------------------------------
    Image();


    //--------------------------------------------------------------------------
    /// Copy constructor: Copy an existing image
    /**
    * @param anImage: The image to copy
    */
    //--------------------------------------------------------------------------
    Image(const Image& anImage);


    //--------------------------------------------------------------------------
    /// Constructor: Copy an 1D array
    /**
    * @param anImage: The pixel data
    * @param aWidth: The image width
    * @param aHeight: The image height
    */
    //--------------------------------------------------------------------------
    Image(const float* anImage, size_t aWidth, size_t aHeight);


    //--------------------------------------------------------------------------
    /// Constructor: Copy an 1D array
    /**
    * @param anImage: The pixel data
    * @param aWidth: The image width
    * @param aHeight: The image height
    */
    //--------------------------------------------------------------------------
    Image(const std::vector<float>& anImage, size_t aWidth, size_t aHeight);


    //--------------------------------------------------------------------------
    /// Constructor: Copy a uniform image from a constant value
    /**
    * @param aConstant: The default pixel value
    * @param aWidth: The image width
    * @param aHeight: The image height
    */
    //--------------------------------------------------------------------------
    Image(float aConstant, size_t aWidth, size_t aHeight);


    //--------------------------------------------------------------------------
    /// Constructor: Load a file from the disk
    /**
    * @param aFilename: The name of the file to load
    */
    //--------------------------------------------------------------------------
    Image(const char* aFilename);


    //--------------------------------------------------------------------------
    /// Constructor: Load a file from the disk
    /**
    * @param aFilename: The name of the file to load
    */
    //--------------------------------------------------------------------------
    Image(const std::string& aFilename);


    //--------------------------------------------------------------------------
    /// Assignment operator
    /**
    * @param anInputImage: The image to copy
    * @return the new image
    */
    //--------------------------------------------------------------------------
    Image& operator=(const Image& anInputImage);


    //--------------------------------------------------------------------------
    /// Assignment operator
    /**
    * @param aFilename: The name of the file to load
    * @return the new image
    */
    //--------------------------------------------------------------------------
    Image& operator=(const char* aFileName);


    //--------------------------------------------------------------------------
    /// Assignment operator
    /**
    * @param aFilename: The name of the file to load
    * @return the new image
    */
    //--------------------------------------------------------------------------
    Image& operator=(const std::string& aFileName);


    //--------------------------------------------------------------------------
    /// Constructor: Load a file from the disk
    /**
    * @param aFilename: The name of the file to load
    */
    //--------------------------------------------------------------------------
    void load(const char* aFilename);


    //--------------------------------------------------------------------------
    /// Constructor: Load a file from the disk
    /**
    * @param aFilename: The name of the file to load
    */
    //--------------------------------------------------------------------------
    void load(const std::string& aFilename);


    //--------------------------------------------------------------------------
    /// Accessor on a given pixel
    /**
    * @param col: coordinate of the pixel along the horizontal axis
    * @param row: coordinate of the pixel along the vertical axis
    * @return the corresponding pixel value
    */
    //--------------------------------------------------------------------------
    const float& operator()(size_t col, size_t row) const;


    //--------------------------------------------------------------------------
    /// Accessor on a given pixel
    /**
    * @param col: coordinate of the pixel along the horizontal axis
    * @param row: coordinate of the pixel along the vertical axis
    * @return the corresponding pixel value
    */
    //--------------------------------------------------------------------------
    float& operator()(size_t col, size_t row);


    //--------------------------------------------------------------------------
    /// Accessor on the image width in number of pixels
    /**
    * @return the width of the image in number of pixels
    */
    //--------------------------------------------------------------------------
    size_t getWidth() const;


    //--------------------------------------------------------------------------
    /// Accessor on the image height in number of pixels
    /**
    * @return the height of the image in number of pixels
    */
    //--------------------------------------------------------------------------
    size_t getHeight() const;


    //--------------------------------------------------------------------------
    /// Accessor on the raw pixel data
    /**
    * @return the pointer on the raw pixel data
    */
    //--------------------------------------------------------------------------
    const float* getPixelPointer() const;


    //--------------------------------------------------------------------------
    /// Accessor on the raw pixel data
    /**
    * @return the pointer on the raw pixel data
    */
    //--------------------------------------------------------------------------
    float* getPixelPointer();


private:
    std::vector<float> m_pixel_data; //< The pixel data in greyscale as a 1D array (here STL vector)
    size_t m_width; //< The number of columns
    size_t m_height; //< The number of rows
    float m_min_pixel_value; //< The smallest pixel value
    float m_max_pixel_value; //< The largest pixel value
    float m_average_pixel_value; //< The average pixel value
    float m_stddev_pixel_value; //< The standard deviation of the pixel values

    bool m_stats_up_to_date; //< True if m_min_pixel_value, m_max_pixel_value, m_average_pixel_value and m_stddev_pixel_value are up-to-date, false otherwise
};

#endif // __Image_h
