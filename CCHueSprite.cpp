//
//  CCHueSprite.cpp
//  cocos2d_libs
//
//  Created by SongofShadow on 15/6/11.
//
//
#include "CCHueSprite.h"
#include "2d/CCSpriteFrame.h"
#include "2d/CCSpriteFrameCache.h"
#include "cocos2d.h"

NS_CC_BEGIN

HueSprite::HueSprite(void)
:_hueAngle(0.0f)
,_programKey("HueSpriteDefault")
,_isETC(false)
,_finalRGBMatrix(Mat4::IDENTITY)
,_targetColor(Vec3(24, 43, 104))
,_colorTestEnabled(true)
{
    setOriginColor(Vec3(24, 43, 104));
}


HueSprite::~HueSprite(void)
{
    
}

HueSprite* HueSprite::createWithTexture(Texture2D *texture)
{
    HueSprite *sprite = new (std::nothrow) HueSprite();
    if (sprite && sprite->initWithTexture(texture))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

HueSprite* HueSprite::createWithTexture(Texture2D *texture, const Rect& rect, bool rotated)
{
    HueSprite *sprite = new (std::nothrow) HueSprite();
    if (sprite && sprite->initWithTexture(texture, rect, rotated))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

HueSprite* HueSprite::create()
{
    HueSprite *sprite = new (std::nothrow) HueSprite();
    if (sprite && sprite->init())
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

HueSprite* HueSprite::create(const std::string& filename)
{
    HueSprite *sprite = new (std::nothrow) HueSprite();
    if (sprite && sprite->initWithFile(filename))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

HueSprite* HueSprite::create(const std::string& filename, const Rect& rect)
{
    HueSprite *sprite = new (std::nothrow) HueSprite();
    if (sprite && sprite->initWithFile(filename, rect))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

HueSprite* HueSprite::createWithSpriteFrame(SpriteFrame *spriteFrame)
{
    HueSprite *sprite = new (std::nothrow) HueSprite();
    if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

HueSprite* HueSprite::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    SpriteFrame *frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
    
#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
    CCASSERT(frame != nullptr, msg);
#endif
    
    return createWithSpriteFrame(frame);
}

bool HueSprite::initWithTexture(Texture2D *texture)
{
    CCASSERT(texture != nullptr, "Invalid texture for sprite");
    
    Rect rect = Rect::ZERO;
    rect.size = texture->getContentSize();
    
    return initWithTexture(texture, rect);
}

bool HueSprite::initWithTexture(Texture2D *texture, const Rect& rect)
{
    return initWithTexture(texture, rect, false);
}

bool HueSprite::initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect &rect, bool rotated)
{
    bool result;
    if (Node::init())
    {
        _batchNode = nullptr;
        
        _recursiveDirty = false;
        setDirty(false);
        
        _opacityModifyRGB = true;
        
        _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
        
        _flippedX = _flippedY = false;
        
        // default transform anchor: center
        setAnchorPoint(Vec2(0.5f, 0.5f));
        
        // zwoptex default values
        _offsetPosition = Vec2::ZERO;
        
        // clean the Quad
        memset(&_quad, 0, sizeof(_quad));
        
        // Atlas: Color
        _quad.bl.colors = Color4B::WHITE;
        _quad.br.colors = Color4B::WHITE;
        _quad.tl.colors = Color4B::WHITE;
        _quad.tr.colors = Color4B::WHITE;
        
        // shader state
        //setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP));
        if (texture && _isETC != texture->getIsETC()) {
            _isETC = texture->getIsETC();
            initShader(true);
        }
        else {
            initShader();
        }
        
        // update texture (calls updateBlendFunc)
        setTexture(texture);
        setTextureRect(rect, rotated, rect.size);
        
        // by default use "Self Render".
        // if the sprite is added to a batchnode, then it will automatically switch to "batchnode Render"
        setBatchNode(nullptr);
        result = true;
    }
    else
    {
        result = false;
    }
    _recursiveDirty = true;
    setDirty(true);
    return result;
}

void HueSprite::setSpriteFrame(SpriteFrame *spriteFrame)
{
    // retain the sprite frame
    // do not removed by SpriteFrameCache::removeUnusedSpriteFrames
    if (_spriteFrame != spriteFrame)
    {
        CC_SAFE_RELEASE(_spriteFrame);
        _spriteFrame = spriteFrame;
        spriteFrame->retain();
    }
    _unflippedOffsetPositionFromCenter = spriteFrame->getOffset();
    
    Texture2D *texture = spriteFrame->getTexture();
    // update texture before updating texture rect
    if (texture != _texture)
    {
        setTexture(texture);
    }
    
    //added by XuTangzisong for pkm support
    if (texture && _isETC != texture->getIsETC()) {
        _isETC = texture->getIsETC();
        initShader(true);
    }
    
    // update rect
    _rectRotated = spriteFrame->isRotated();
    setTextureRect(spriteFrame->getRect(), _rectRotated, spriteFrame->getOriginalSize());
}

const GLchar* HueSprite::getFragmentShader()
{
    
    const GLchar* fragmentShader =
    " \
    \n#define COLOR_TEST\n \
    \
    \n#ifdef GL_ES\n \
    precision mediump float; \
    \n#endif\n \
    varying vec2 v_texCoord; \
    varying vec4 v_fragmentColor; \
    uniform mat4 u_finalRGBMatrix; \
    \
    void main() \
    { \
        vec4 textureColor = texture2D(CC_Texture0, v_texCoord); \
        vec4 newTextureColor = textureColor; \
        //color test, only blue fragment need convert \
        \n#ifdef COLOR_TEST\n \
        if (newTextureColor.r < 90.0/255.0 && newTextureColor.b > 90.0/255.0) { \
            \n#endif\n \
            newTextureColor = u_finalRGBMatrix * textureColor; \
            \n#ifdef COLOR_TEST\n \
        } \
        \n#endif\n \
        gl_FragColor = v_fragmentColor*newTextureColor; \
    } \
    ";
    const GLchar* fragmentShaderNOTEST =
    " \
    \n#ifdef GL_ES\n \
    precision mediump float; \
    \n#endif\n \
    varying vec2 v_texCoord; \
    varying vec4 v_fragmentColor; \
    uniform mat4 u_finalRGBMatrix; \
    \
    void main() \
    { \
        vec4 textureColor = texture2D(CC_Texture0, v_texCoord); \
        vec4 newTextureColor = textureColor; \
        //color test, only blue fragment need convert \
        \n#ifdef COLOR_TEST\n \
        if (newTextureColor.r < 90.0/255.0 && newTextureColor.b > 90.0/255.0) { \
            \n#endif\n \
            newTextureColor = u_finalRGBMatrix * textureColor; \
            \n#ifdef COLOR_TEST\n \
        } \
        \n#endif\n \
        gl_FragColor = v_fragmentColor*newTextureColor; \
    } \
    ";
    const GLchar* fragmentETCShaderNOTEST =
    " \
    \n#ifdef GL_ES\n \
    precision mediump float; \
    \n#endif\n \
    varying vec2 v_texCoord; \
    varying vec4 v_fragmentColor; \
    uniform mat4 u_finalRGBMatrix; \
    \
    void main() \
    { \
        vec4 textureColor = vec4(texture2D(CC_Texture0, v_texCoord).xyz, texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 0.5)).x); \
        vec4 newTextureColor = textureColor; \
        //color test, only blue fragment need convert \
        \n#ifdef COLOR_TEST\n \
        if (newTextureColor.r < 90.0/255.0 && newTextureColor.b > 90.0/255.0) { \
            \n#endif\n \
            newTextureColor = u_finalRGBMatrix * textureColor; \
            \n#ifdef COLOR_TEST\n \
        } \
        \n#endif\n \
        gl_FragColor = v_fragmentColor*newTextureColor; \
    } \
    ";
    const GLchar* fragmentETCShader =
    " \
    \n#define COLOR_TEST\n \
    \
    \n#ifdef GL_ES\n \
    precision mediump float; \
    \n#endif\n \
    varying vec2 v_texCoord; \
    varying vec4 v_fragmentColor; \
    uniform mat4 u_finalRGBMatrix; \
    \
    void main() \
    { \
        vec4 textureColor = vec4(texture2D(CC_Texture0, v_texCoord).xyz, texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y + 0.5)).x); \
        vec4 newTextureColor = textureColor; \
        //color test, only blue fragment need convert \
        \n#ifdef COLOR_TEST\n \
        if (newTextureColor.r < 90.0/255.0 && newTextureColor.b > 90.0/255.0) { \
            \n#endif\n \
            newTextureColor = u_finalRGBMatrix * textureColor; \
            \n#ifdef COLOR_TEST\n \
        } \
        \n#endif\n \
        gl_FragColor = v_fragmentColor*newTextureColor; \
    } \
    ";
    if (_isETC) {
        if (!_colorTestEnabled) {
            return fragmentETCShaderNOTEST;
        }
        return fragmentETCShader;
        
    }
    if (!_colorTestEnabled) {
        return fragmentShaderNOTEST;
    }
    return fragmentShader;
}

const GLchar* HueSprite::getVertexShader()
{
    const GLchar* ccPositionTextureColor_noMVP_vert =
    " \
    attribute vec4 a_position; \
    attribute vec2 a_texCoord; \
    attribute vec4 a_color; \
    \
    \n#ifdef GL_ES\n \
    varying lowp vec4 v_fragmentColor; \
    varying mediump vec2 v_texCoord; \
    \n#else\n \
        varying vec4 v_fragmentColor; \
    varying vec2 v_texCoord; \
    \n#endif\n \
    \
    void main() \
    { \
        gl_Position = CC_PMatrix * a_position; \
        v_fragmentColor = a_color; \
        v_texCoord = a_texCoord; \
    } \
    ";
    return ccPositionTextureColor_noMVP_vert;
}

//加载shader
void HueSprite::initShader(bool needRefresh)
{
    auto glProgramState = GLProgramState::getOrCreateWithGLProgramName(_programKey);
    if (glProgramState && !needRefresh) {
        setGLProgramState(glProgramState);
    }
    else {
        auto glProgram = GLProgram::createWithByteArrays(getVertexShader(), getFragmentShader());
        setGLProgram(glProgram);
        setGLProgramState(GLProgramState::getOrCreateWithGLProgram(glProgram));
        GLProgramCache::getInstance()->addGLProgram(glProgram, _programKey);
    }
    setUniformRGBMatrix();
}

void HueSprite::setProgramKey(std::string key)
{
    _programKey.assign(key);
    initShader();
}

void HueSprite::setOpacity(GLubyte opacity)
{
    Sprite::setOpacity(opacity);
}

//向fragment传入新rgb变换矩阵
void HueSprite::setUniformRGBMatrix()
{
    //色相旋转
    rotateHue();
    getGLProgramState()->setUniformMat4("u_finalRGBMatrix", _finalRGBMatrix);
}

//默认为蓝色，色相角度240
void HueSprite::setOriginColor(Vec3 originColor)
{
    float hue = getHueByRGB(originColor);
    _originHueAngle = hue;
}

void HueSprite::setOriginColor(int r, int g, int b)
{
    setOriginColor(Vec3(r, g ,b));
}

//计算和默认角度的差值，作为hueAngle
void HueSprite::setTargetColor(Vec3 targetColor)
{
    _targetColor = targetColor;
    float hue = getHueByRGB(targetColor);
    setHueAngle(hue - _originHueAngle);
}

void HueSprite::setTargetColor(int r, int g, int b)
{
    setTargetColor(Vec3(r, g ,b));
}

void HueSprite::setHueAngle(float hueAngle)
{
    _hueAngle = hueAngle;
    setUniformRGBMatrix();
}

void HueSprite::rotateHue()
{
    Mat4::createRotation(Vec3(1.0, 1.0, 1.0), _hueAngle, &_finalRGBMatrix);
}

float HueSprite::getHueByRGB(Vec3 color)
{
    int r = color.x;
    int g = color.y;
    int b = color.z;
    
    int max = 0;
    int min = 0;
    if (r >= g && r >= b) {
        max = r;
        if (g >= b) {
            min = b;
        }
        else {
            min = g;
        }
    }
    if (g >= r && g >= b) {
        max = g;
        if (r >= b) {
            min = b;
        }
        else {
            min = r;
        }
    }
    if (b >= r && b >= g) {
        max = b;
        if (r >= g) {
            min = g;
        }
        else {
            min = r;
        }
    }
    
    float hue = 0;
    if (max == r) {
        hue = (g - b)*60.0f/(max - min) + 0;
    }
    else if (max == g) {
        hue = (b - r)*60.0f/(max - min) + 120;
    }
    else if (max == b) {
        hue = (r - g)*60.0f/(max - min) + 240;
    }
    return hue/180*M_PI;
}

float HueSprite::getSaturationByRGB(Vec3 color)
{
    int r = color.x;
    int g = color.y;
    int b = color.z;
    
    int max = 0;
    int min = 0;
    if (r >= g && r >= b) {
        max = r;
        if (g >= b) {
            min = b;
        }
        else {
            min = g;
        }
    }
    if (g >= r && g >= b) {
        max = g;
        if (r >= b) {
            min = b;
        }
        else {
            min = r;
        }
    }
    if (b >= r && b >= g) {
        max = b;
        if (r >= g) {
            min = g;
        }
        else {
            min = r;
        }
    }
    
    float saturation = 0.0;
    if (max - 0.0 < FLT_MIN) {
        //=0.0
    }
    else {
        saturation = 1.0 - min*1.0/max;
    }
    return saturation;
}

float HueSprite::getBrightnessByRGB(Vec3 color)
{
    int r = color.x;
    int g = color.y;
    int b = color.z;
    
    int max = 0;
    int min = 0;
    if (r >= g && r >= b) {
        max = r;
        if (g >= b) {
            min = b;
        }
        else {
            min = g;
        }
    }
    if (g >= r && g >= b) {
        max = g;
        if (r >= b) {
            min = b;
        }
        else {
            min = r;
        }
    }
    if (b >= r && b >= g) {
        max = b;
        if (r >= g) {
            min = g;
        }
        else {
            min = r;
        }
    }
    
    return (float)max;
}

NS_CC_END