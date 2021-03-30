#pragma once

#include "../WndDesign/WndDesign.h"
#include "../WndDesign/figure/text_block.h"
#include "../WndDesign/message/timer.h"
#include "../WndDesign/system/win32_aero_snap.h"

#include <vector>
#include <random>


using namespace WndDesign;


class MainWnd : public WndObject {
private:
	static constexpr Rect region = Rect(200, 100, 800, 500);
private:
	virtual const Rect UpdateRegionOnParent(Size parent_size) override { return region; }
	virtual const pair<Size, Size> CalculateMinMaxSize(Size parent_size) { return { region.size, region.size }; }
	virtual const wstring GetTitle() const { return L"Figure Test"; }

private:
	static uint Square(int number) { return (uint)(number * number); }
	static uint CalculateParticleRadius(Vector velocity) {
		return 20 - 15 * (Square(velocity.x) + Square(velocity.y)) / 1800;
	}
private:
	struct ParticleData {
		Point point;
		Color color;
		Vector velocity;
		uint radius;
		ParticleData(Point point) :
			point(point),
			color(rand() % 256, rand() % 256, rand() % 256),
			velocity(Vector(rand() % 61 - 30, rand() % 61 - 30)),
			radius(CalculateParticleRadius(velocity)) {
		}
	};
private:
	std::vector<ParticleData> particles;
	Timer timer = Timer([&]() {UpdateParticle(); });
private:
	void AddParticle(Point point) {
		for (int i = 0; i < 10; ++i) {
			particles.emplace_back(point);
		}
		text = L"Particle Count: " + std::to_wstring(particles.size()); text_block.TextChanged();
	}
	void UpdateParticle() {
		for (auto& particle : particles) {
			particle.point += particle.velocity;
			int& x = particle.point.x; int& y = particle.point.y;
			int& vx = particle.velocity.x; int& vy = particle.velocity.y;
			int width = (int)region.size.width; int height = (int)region.size.height;
			if (x < 0) { x = -x; vx = -vx; } if (x > width) { x = 2 * width - x; vx = -vx; } assert(x >= 0 && x <= width);
			if (y < 0) { y = -y; vy = -vy; } if (y > height) { y = 2 * height - y; vy = -vy; } assert(y >= 0 && y <= height);
		}
		Invalidate(region_infinite);
	}

private:
	struct ParticleFigure : public Figure {
		MainWnd& main_wnd; ParticleFigure(MainWnd& main_wnd) : main_wnd(main_wnd) {}
		virtual const Rect GetRegion() const { return main_wnd.region; }
		virtual void DrawOn(RenderTarget& target, Vector offset) const {
			for (auto& particle : main_wnd.particles) {
				Circle circle(particle.radius, particle.color);
				circle.DrawOn(target, particle.point + offset - point_zero);
			}
		}
	}particle_figure = ParticleFigure(*this);

private:
	wstring text = L"Particle Count: 0";
	TextBlockStyle style;
	TextBlock text_block = TextBlock(text, style);

private:
	virtual void OnComposite(FigureQueue& figure_queue, Size display_size, Rect invalid_display_region) const {
		figure_queue.Append(point_zero, new Rectangle(display_size, ColorSet::White));

	#ifdef FIGURE_TYPE_SWITCHER
		for (auto& particle : particles) {
			figure_queue.Append(particle.point, new Circle(particle.radius, particle.color));
		}
	#else
		figure_queue.Append(point_zero, new ParticleFigure(particle_figure));
	#endif

		figure_queue.Append(point_zero, new TextBlockFigure(text_block));
	}
	virtual void NonClientHandler(Msg msg, Para para) override {
		if (msg == Msg::LeftDown) { AeroSnapDraggingEffect(*this, GetMouseMsg(para).point); }
		if (msg == Msg::RightDown) { AddParticle(GetMouseMsg(para).point); }
	}
};

int main() {
	MainWnd main_wnd;
	desktop.AddChild(main_wnd);
	desktop.MessageLoop();
	return 0;
}