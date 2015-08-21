#ifndef __NV_DDS_H__
#define __NV_DDS_H__

#ifndef NVDDS_PROJECT //defined if building nv_dds library
#if _MSC_VER >= 1300
    #ifdef _DLL
        #pragma message("Note: including lib: nv_dds.lib\n")
        #pragma comment(lib, "nv_dds.lib")
    #else
        #error "Your project doesn't use the Multithreaded DLL Runtime"
    #endif
#endif
#endif


#if defined(WIN32)
#  include <windows.h>
#endif

#include <string>
#include <deque>
#include <assert.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

namespace nv_dds
{
    // surface description flags
    const unsigned long DDSF_CAPS           = 0x00000001l;
    const unsigned long DDSF_HEIGHT         = 0x00000002l;
    const unsigned long DDSF_WIDTH          = 0x00000004l;
    const unsigned long DDSF_PITCH          = 0x00000008l;
    const unsigned long DDSF_PIXELFORMAT    = 0x00001000l;
    const unsigned long DDSF_MIPMAPCOUNT    = 0x00020000l;
    const unsigned long DDSF_LINEARSIZE     = 0x00080000l;
    const unsigned long DDSF_DEPTH          = 0x00800000l;

    // pixel format flags
    const unsigned long DDSF_ALPHAPIXELS    = 0x00000001l;
    const unsigned long DDSF_FOURCC         = 0x00000004l;
    const unsigned long DDSF_RGB            = 0x00000040l;
    const unsigned long DDSF_RGBA           = 0x00000041l;

    // dwCaps1 flags
    const unsigned long DDSF_COMPLEX         = 0x00000008l;
    const unsigned long DDSF_TEXTURE         = 0x00001000l;
    const unsigned long DDSF_MIPMAP          = 0x00400000l;

    // dwCaps2 flags
    const unsigned long DDSF_CUBEMAP         = 0x00000200l;
    const unsigned long DDSF_CUBEMAP_POSITIVEX  = 0x00000400l;
    const unsigned long DDSF_CUBEMAP_NEGATIVEX  = 0x00000800l;
    const unsigned long DDSF_CUBEMAP_POSITIVEY  = 0x00001000l;
    const unsigned long DDSF_CUBEMAP_NEGATIVEY  = 0x00002000l;
    const unsigned long DDSF_CUBEMAP_POSITIVEZ  = 0x00004000l;
    const unsigned long DDSF_CUBEMAP_NEGATIVEZ  = 0x00008000l;
    const unsigned long DDSF_CUBEMAP_ALL_FACES  = 0x0000FC00l;
    const unsigned long DDSF_VOLUME          = 0x00200000l;

    // compressed texture types
    const unsigned long FOURCC_DXT1 = 0x31545844l; //(MAKEFOURCC('D','X','T','1'))
    const unsigned long FOURCC_DXT3 = 0x33545844l; //(MAKEFOURCC('D','X','T','3'))
    const unsigned long FOURCC_DXT5 = 0x35545844l; //(MAKEFOURCC('D','X','T','5'))

    struct DXTColBlock
    {
        unsigned short col0;
        unsigned short col1;

        unsigned char row[4];
    };

    struct DXT3AlphaBlock
    {
        unsigned short row[4];
    };

    struct DXT5AlphaBlock
    {
        unsigned char alpha0;
        unsigned char alpha1;

        unsigned char row[6];
    };

    struct DDS_PIXELFORMAT
    {
        unsigned long dwSize;
        unsigned long dwFlags;
        unsigned long dwFourCC;
        unsigned long dwRGBBitCount;
        unsigned long dwRBitMask;
        unsigned long dwGBitMask;
        unsigned long dwBBitMask;
        unsigned long dwABitMask;
    };

    struct DDS_HEADER
    {
        unsigned long dwSize;
        unsigned long dwFlags;
        unsigned long dwHeight;
        unsigned long dwWidth;
        unsigned long dwPitchOrLinearSize;
        unsigned long dwDepth;
        unsigned long dwMipMapCount;
        unsigned long dwReserved1[11];
        DDS_PIXELFORMAT ddspf;
        unsigned long dwCaps1;
        unsigned long dwCaps2;
        unsigned long dwReserved2[3];
    };

    typedef enum TextureType
    {
        TextureNone,
        TextureFlat,    // 1D, 2D, and rectangle textures
        Texture3D,
        TextureCubemap
    };

    class CSurface
    {
        public:
            CSurface();
            CSurface(unsigned int w, unsigned int h, unsigned int d, unsigned int imgsize, const unsigned char *pixels);
            CSurface(const CSurface &copy);
            CSurface &operator= (const CSurface &rhs);
            virtual ~CSurface();

            operator unsigned char*() const;

            virtual void create(unsigned int w, unsigned int h, unsigned int d, unsigned int imgsize, const unsigned char *pixels);
            virtual void clear();

            inline unsigned int get_width() const { return m_width; }
            inline unsigned int get_height() const { return m_height; }
            inline unsigned int get_depth() const { return m_depth; }
            inline unsigned int get_size() const { return m_size; }

        private:
            unsigned int m_width;
            unsigned int m_height;
            unsigned int m_depth;
            unsigned int m_size;

            unsigned char *m_pixels;
    };

    class CTexture : public CSurface
    {
        friend class CDDSImage;

        public:
            CTexture();
            CTexture(unsigned int w, unsigned int h, unsigned int d, unsigned int imgsize, const unsigned char *pixels);
            CTexture(const CTexture &copy);
            CTexture &operator= (const CTexture &rhs);
            ~CTexture();

            void create(unsigned int w, unsigned int h, unsigned int d, unsigned int imgsize, const unsigned char *pixels);
            void clear();

            inline const CSurface &get_mipmap(unsigned int index) const
            {
                assert(!m_mipmaps.empty());
                assert(index < m_mipmaps.size());

                return m_mipmaps[index];
            }

            inline void add_mipmap(const CSurface &mipmap)
            {
                m_mipmaps.push_back(mipmap);
            }

            inline unsigned int get_num_mipmaps() const { return (unsigned int)m_mipmaps.size(); }

        protected:
            inline CSurface &get_mipmap(unsigned int index)
            {
                assert(!m_mipmaps.empty());
                assert(index < m_mipmaps.size());

                return m_mipmaps[index];
            }

        private:
            std::deque<CSurface> m_mipmaps;
    };

    class CDDSImage
    {
        public:
            CDDSImage();
            ~CDDSImage();

            void create_textureFlat(unsigned int format, unsigned int components, const CTexture &baseImage);
            void create_texture3D(unsigned int format, unsigned int components, const CTexture &baseImage);
            void create_textureCubemap(unsigned int format, unsigned int components,
                                       const CTexture &positiveX, const CTexture &negativeX,
                                       const CTexture &positiveY, const CTexture &negativeY,
                                       const CTexture &positiveZ, const CTexture &negativeZ);

            void clear();
            bool load(std::string filename, bool flipImage = true);
            bool save(std::string filename, bool flipImage = true);

            bool upload_texture1D();
            bool upload_texture2D(unsigned int imageIndex = 0, GLenum target = GL_TEXTURE_2D);
            bool upload_texture3D();
            bool upload_textureRectangle();
            bool upload_textureCubemap();

            inline operator unsigned char*()
            {
                assert(m_valid);
                assert(!m_images.empty());

                return m_images[0];
            }

            inline unsigned int get_width()
            {
                assert(m_valid);
                assert(!m_images.empty());

                return m_images[0].get_width();
            }

            inline unsigned int get_height()
            {
                assert(m_valid);
                assert(!m_images.empty());

                return m_images[0].get_height();
            }

            inline unsigned int get_depth()
            {
                assert(m_valid);
                assert(!m_images.empty());

                return m_images[0].get_depth();
            }

            inline unsigned int get_size()
            {
                assert(m_valid);
                assert(!m_images.empty());

                return m_images[0].get_size();
            }

            inline unsigned int get_num_mipmaps()
            {
                assert(m_valid);
                assert(!m_images.empty());

                return m_images[0].get_num_mipmaps();
            }

            inline const CSurface &get_mipmap(unsigned int index) const
            {
                assert(m_valid);
                assert(!m_images.empty());
                assert(index < m_images[0].get_num_mipmaps());

                return m_images[0].get_mipmap(index);
            }

            inline const CTexture &get_cubemap_face(unsigned int face) const
            {
                assert(m_valid);
                assert(!m_images.empty());
                assert(m_images.size() == 6);
                assert(m_type == TextureCubemap);
                assert(face < 6);

                return m_images[face];
            }

            inline unsigned int get_components() { return m_components; }
            inline unsigned int get_format() { return m_format; }
            inline TextureType get_type() { return m_type; }

            inline bool is_compressed()
            {
                if ((m_format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ||
                    (m_format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) ||
                    (m_format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT))
                    return true;
                else
                    return false;
            }

            inline bool is_cubemap() { return (m_type == TextureCubemap); }
            inline bool is_volume() { return (m_type == Texture3D); }
            inline bool is_valid() { return m_valid; }

            inline bool is_dword_aligned()
            {
                assert(m_valid);

                int dwordLineSize = get_dword_aligned_linesize(get_width(), m_components*8);
                int curLineSize = get_width() * m_components;

                return (dwordLineSize == curLineSize);
            }

        private:
            unsigned int clamp_size(unsigned int size);
            unsigned int size_dxtc(unsigned int width, unsigned int height);
            unsigned int size_rgb(unsigned int width, unsigned int height);
            inline void swap_endian(void *val);

            // calculates 4-byte aligned width of image
            inline unsigned int get_dword_aligned_linesize(unsigned int width, unsigned int bpp)
            {
                return ((width * bpp + 31) & -32) >> 3;
            }

            void flip(CSurface &surface);
            void flip_texture(CTexture &texture);

            void swap(void *byte1, void *byte2, unsigned int size);
            void flip_blocks_dxtc1(DXTColBlock *line, unsigned int numBlocks);
            void flip_blocks_dxtc3(DXTColBlock *line, unsigned int numBlocks);
            void flip_blocks_dxtc5(DXTColBlock *line, unsigned int numBlocks);
            void flip_dxt5_alpha(DXT5AlphaBlock *block);

            void write_texture(const CTexture &texture, FILE *fp);

            unsigned int m_format;
            unsigned int m_components;
            TextureType m_type;
            bool m_valid;

            std::deque<CTexture> m_images;

#ifndef __APPLE__
            static PFNGLTEXIMAGE3DEXTPROC glTexImage3D;
            static PFNGLCOMPRESSEDTEXIMAGE1DARBPROC glCompressedTexImage1DARB;
            static PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB;
            static PFNGLCOMPRESSEDTEXIMAGE3DARBPROC glCompressedTexImage3DARB;
#endif
    };
}
#endif
