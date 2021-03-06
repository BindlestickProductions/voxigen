#ifndef _voxigen_simpleChunkRenderer_h_
#define _voxigen_simpleChunkRenderer_h_

#include "voxigen/voxigen_export.h"
#include "voxigen/chunk.h"
#include "voxigen/chunkHandle.h"
#include "voxigen/chunkInfo.h"
#include "voxigen/chunkVolume.h"
#include "voxigen/cubicMeshBuilder.h"

//#include "PolyVox/CubicSurfaceExtractor.h"
//#include "PolyVox/Mesh.h"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace voxigen
{

/////////////////////////////////////////////////////////////////////////////////////////
//SimpleChunkRenderer
/////////////////////////////////////////////////////////////////////////////////////////
//template<typename _Block>
template<typename _Parent, typename _Chunk>
class SimpleChunkRenderer
{
public:
    typedef _Parent RenderType;
    typedef _Chunk ChunkType;
    typedef ChunkHandle<ChunkType> ChunkHandleType;
    typedef std::shared_ptr<ChunkHandleType> SharedChunkHandle;

//    typedef PolyVox::CubicVertex<typename ChunkVolume<_Chunk>::VoxelType> CubicVertex;
//    typedef PolyVox::Mesh<CubicVertex> MeshType;
//    typedef std::shared_ptr<ChunkType> SharedChunk;

    SimpleChunkRenderer();
    ~SimpleChunkRenderer();
    
    enum State
    {
        Init,
        Invalid,
        Occluded,
        Query, 
        QueryWait,
        Dirty,
        Copy,
        Built,
        Empty
    };

    State getState() { return m_state; }

    void setParent(RenderType *parent);
//    void setRegionHash(RegionHash hash);
    void setChunk(SharedChunkHandle chunk);
    void setEmpty();
//    void setChunkOffset(glm::vec3 chunkOffset);
    const glm::vec3 &getChunkOffset() { return m_chunkOffset; }

    void build(unsigned int instanceData);
    void buildOutline(unsigned int instanceData);

    void update();
    void updated();

    void updateOutline();
    void invalidate();

    bool incrementCopy();
    void draw();

    void startOcculsionQuery();
    void drawOcculsionQuery();
    bool checkOcculsionQuery(unsigned int &samples);

//#ifndef NDEBUG
        void drawOutline();
//#endif //NDEBUG

    const RegionHash getRegionHash() { return m_chunkHandle->regionHash; }
    const ChunkHash getChunkHash() { return m_chunkHandle->hash; }
    SharedChunkHandle getChunkHandle() { return m_chunkHandle; }
    const glm::ivec3 &getPosition() { return m_chunkHandle->chunk->getPosition(); }
    glm::vec3 getGridOffset() const { return m_chunkHandle->regionOffset;/* m_chunkHandle->chunk->getGridOffset();*/ }
    
    unsigned int refCount;
private:
    RenderType *m_parent;

    State m_state;
//    RegionHash m_regionHash;
    SharedChunkHandle m_chunkHandle;

 //   bool m_empty;

//#ifndef NDEBUG
    bool m_outlineBuilt;
//#endif
    unsigned int m_queryId;

    unsigned int m_vertexBuffer;
    unsigned int m_indexBuffer;
    GLenum m_indexType;

    glm::vec3 m_chunkOffset;
    unsigned int m_validBlocks;
    unsigned int m_vertexArray;
    unsigned int m_offsetVBO;
    std::vector<glm::vec4> m_ChunkInfoOffset;

//    MeshType m_mesh;
    ChunkMesh m_mesh;

    GLsync m_vertexBufferSync;
    int m_delayedFrames;

    unsigned int m_outlineVertexArray;
    unsigned int m_outlineOffsetVBO;
};

}//namespace voxigen

#include "simpleChunkRenderer.inl"

#endif //_voxigen_simpleChunkRenderer_h_