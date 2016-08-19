#ifndef ANIMATION_BASE_H
#define ANIMATION_BASE_H

#include <sstream>
#include <string>

//enum class Animation { Walk, Jump, Attack, Hurt, Death, Idle };

class SpriteSheet;
class AnimationBase
{
	friend std::stringstream& operator >> (std::stringstream& stream, AnimationBase& a)
	{
		a.readIn(stream);
		return stream;
	}
	friend class SpriteSheet;
public:
	AnimationBase();
	virtual ~AnimationBase();

	std::string getName() const { return m_name; }
	bool isPlaying() const { return m_playing; }

	void setName(const std::string& name) { m_name = name; }
	void setSpriteSheet(SpriteSheet* spriteSheet) { m_spriteSheet = spriteSheet; }
	void setFrame(const unsigned int frame);
	bool isInAction();
	void setLooping(const bool loop) { m_loop = loop; }
	void play() { m_playing = true; }
	void pause() { m_playing = false; }
	void stop() { m_playing = false; reset(); }
	void reset();

	virtual void update(const float deltaTime);

protected:
	virtual void frameStep() = 0; //Updates the portion that is unique to different types of animatons
	virtual void cropSprite() = 0;
	virtual void readIn(std::stringstream& keyStream) = 0;

	unsigned int m_frameCurrent; //The current frame
	unsigned int m_frameStart; //What frame it starts at
	unsigned int m_frameEnd; //What frame it ends at
	unsigned int m_frameRow; //What row frame is on
	int m_frameActionStart; //Frame when a specific action begins
	int m_frameActionEnd; //Frame when a specific action ends
	float m_frameTime; //How long a frame took
	float m_elaspedTime;
	bool m_loop;
	bool m_playing;
	std::string m_name;
	SpriteSheet* m_spriteSheet;
};
#endif // !ANIMATION_BASE_H
