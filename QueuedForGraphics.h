#pragma once

class QueuedForGraphics
{
public:
	//QueuedForGraphics(void);
	//~QueuedForGraphics(void);
	virtual void run() = 0;
};

class QueuedStarUpdate : public QueuedForGraphics
{
public:
	QueuedStarUpdate();
	void run() override;

private:

};

class QueuedFlyerCrash : public QueuedForGraphics
{
public:
	QueuedFlyerCrash();
	void run() override;

private:

};