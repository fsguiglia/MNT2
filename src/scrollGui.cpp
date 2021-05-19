#include "scrollGui.h"

void ScrollGui::setMaxHeight(int height)
{
	_maxHeight = height;
}

void ScrollGui::scroll(int amount)
{
	int x = getPosition().x;
	int y = getPosition().y;
	if((y + amount <= 0) && (y + getHeight() + amount >= _maxHeight)) setPosition(x, y + amount);
}

void ScrollGui::setRemovableSlider(string label)
{
	ofRectangle removableSlider;
	removableSlider.x = getSlider(label)->getX();
	removableSlider.y = getSlider(label)->getY();
	removableSlider.width = getSlider(label)->getWidth();
	removableSlider.height = getSlider(label)->getHeight();
	if (_removableSliders.find(label) == _removableSliders.end())
	{
		_removableSliders[label] = removableSlider;
	}
}

string ScrollGui::inside(int x, int y)
{
	string inside = "";
	for (auto removableSlider : _removableSliders)
	{
		if (removableSlider.second.inside(x, y))
		{
			inside = removableSlider.first;
			break;
		}
	}
	return inside;
}

void ScrollGui::removeSlider(string label)
{
	if (_removableSliders.find(label) != _removableSliders.end())
	{
		removeComponent(getSlider(label));
		_removableSliders.erase(label);
	}
}

void ScrollGui::updatePositions()
{
	for (auto& removableSlider : _removableSliders)
	{
		removableSlider.second.x = getSlider(removableSlider.first)->getX();
		removableSlider.second.y = getSlider(removableSlider.first)->getY();
		removableSlider.second.width = getSlider(removableSlider.first)->getWidth();
		removableSlider.second.height = getSlider(removableSlider.first)->getHeight();
	}
}

void ScrollGui::clearRemovableSliders()
{
	vector<string> curSliders;
	for (auto removableSlider : _removableSliders) curSliders.push_back(removableSlider.first);
	for (auto removableSlider : curSliders) removeSlider(removableSlider);
}
