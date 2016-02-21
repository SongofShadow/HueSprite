//
//  CCHueSprite.h
//  cocos2d_libs
//
//  Created by SongofShadow on 15/6/11.
//
//

#ifndef __cocos2d_libs__CCHueSprite__
#define __cocos2d_libs__CCHueSprite__
#include "2d/CCSprite.h"

NS_CC_BEGIN

class HueSprite : public Sprite
{
public:
    HueSprite(void);
    virtual ~HueSprite(void);
    
    static HueSprite* create();
    static HueSprite* create(const std::string& filename);
    static HueSprite* create(const std::string& filename, const Rect& rect);
    
    static HueSprite* createWithTexture(Texture2D *texture);
    static HueSprite* createWithTexture(Texture2D *texture, const Rect& rect, bool rotated=false);
    
    static HueSprite* createWithSpriteFrame(SpriteFrame *spriteFrame);
    static HueSprite* createWithSpriteFrameName(const std::string& spriteFrameName);
    
    virtual bool initWithTexture(Texture2D *texture);
    virtual bool initWithTexture(Texture2D *texture, const Rect& rect);
    virtual bool initWithTexture(cocos2d::Texture2D *texture, const cocos2d::Rect &rect, bool rotated);
    virtual void setSpriteFrame(SpriteFrame* newFrame);
    virtual void setOpacity(GLubyte opacity);
    float getHueAngle() {return _hueAngle;};
    void setHueAngle(float hueAngle);
    void setOriginColor(int r, int g, int b);
    void setOriginColor(Vec3 originColor);//默认为蓝色，色相角度240
    void setTargetColor(int r, int g, int b);
    void setTargetColor(Vec3 targetColor);//计算和默认角度的差值，作为hueAngle
    void setProgramKey(std::string key);
    void setColorTestEnabled(bool enabled) {_colorTestEnabled = enabled;};
protected:
    
private:
    void initShader(bool needRefresh = false);//加载shader
    const GLchar* getVertexShader();
    const GLchar* getFragmentShader();
    void setUniformRGBMatrix();//向fragment传入新rgb变换矩阵
    void rotateHue();//完成色相旋转
    GLfloat getAlpha() {return _displayedOpacity/255.0f;};
    float getHueByRGB(Vec3 color);//根据RGB计算色相
    float getSaturationByRGB(Vec3 color);
    float getBrightnessByRGB(Vec3 color);
private:
    //色相旋转角度
    float _originHueAngle;
    float _hueAngle;
    //最终rgb变换矩阵
    Mat4 _finalRGBMatrix;
    std::string _programKey;
    bool _isETC;
    Vec3 _targetColor;
    bool _colorTestEnabled;
};

NS_CC_END

#endif //__HUE_SPRITE_H__

