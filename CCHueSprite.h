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
    void setOriginColor(Vec3 originColor);//Ĭ��Ϊ��ɫ��ɫ��Ƕ�240
    void setTargetColor(int r, int g, int b);
    void setTargetColor(Vec3 targetColor);//�����Ĭ�ϽǶȵĲ�ֵ����ΪhueAngle
    void setProgramKey(std::string key);
    void setColorTestEnabled(bool enabled) {_colorTestEnabled = enabled;};
protected:
    
private:
    void initShader(bool needRefresh = false);//����shader
    const GLchar* getVertexShader();
    const GLchar* getFragmentShader();
    void setUniformRGBMatrix();//��fragment������rgb�任����
    void rotateHue();//���ɫ����ת
    GLfloat getAlpha() {return _displayedOpacity/255.0f;};
    float getHueByRGB(Vec3 color);//����RGB����ɫ��
    float getSaturationByRGB(Vec3 color);
    float getBrightnessByRGB(Vec3 color);
private:
    //ɫ����ת�Ƕ�
    float _originHueAngle;
    float _hueAngle;
    //����rgb�任����
    Mat4 _finalRGBMatrix;
    std::string _programKey;
    bool _isETC;
    Vec3 _targetColor;
    bool _colorTestEnabled;
};

NS_CC_END

#endif //__HUE_SPRITE_H__

