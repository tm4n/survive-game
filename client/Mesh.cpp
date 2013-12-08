#ifndef __MESH_CPP__
#define __MESH_CPP__

Mesh::Mesh(const char *mesh_file, const char *tex_file)
{
		int[] textures = new int[1];
        glGenTextures(1, textures, 0);
        mTextureID = textures[0];
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureID);
        
        // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER,GLES20.GL_LINEAR_MIPMAP_LINEAR);
        // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER,GLES20.GL_LINEAR);
}


#endif