#include <iostream>
#include <SFML/Graphics.hpp>

#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include "Box2D/Dynamics/b2Body.h"
#include "Box2D/Dynamics/b2Fixture.h"
#include "Box2D/Dynamics/b2World.h"

using namespace std;
using namespace sf;

float gameWidth = 800.f;
float gameHeight = 600.f;

b2World* world;

//1 sfml unit = 30 physics units
const float physics_scale = 30.0f;
//inverse of physics_scale, useful for calculations
const float physics_scale_inv = 1.0f / physics_scale;
//magic numbers for accuracy of physics simulation
const int32 velocityIterations = 6;
const int32 positionIterations = 2;

std::vector<b2Body*> bodies;
std::vector<RectangleShape*> sprites;

//convert from b2vec2 to a vector2f
inline const Vector2f bv2_to_sv2(const b2Vec2& in) {
	return Vector2f(in.x * physics_scale, in.y * physics_scale);
}
//convert from vector2f to a b2vec2
inline const b2Vec2 sv2_to_bv2(const Vector2f& in) {
	return b2Vec2(in.x * physics_scale_inv, in.y * physics_scale_inv);
}
//convert from screenspace.y to physics.y (as they are the other way around)
inline const Vector2f invert_height(const Vector2f& in) {
	return Vector2f(in.x, gameHeight - in.y);
}

//create a box2d body with a box fixture
b2Body* CreatePhysicsBox(b2World& World, const bool dynamic, const Vector2f& position, const Vector2f& size) {
	b2BodyDef BodyDef;
	//is dynamic (moving) or static (stationary)
	BodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
	BodyDef.position = sv2_to_bv2(position);
	//create the body
	b2Body* body = World.CreateBody(&BodyDef);

	//create the fixture shape
	b2PolygonShape Shape;
	Shape.SetAsBox(sv2_to_bv2(size).x * 0.5f, sv2_to_bv2(size).y * 0.5f);
	b2FixtureDef FixtureDef;
	//fixture properties
	FixtureDef.density = dynamic ? 10.0f : 0.0f;
	FixtureDef.friction = dynamic ? .8f : 1.f;
	FixtureDef.restitution = 1.0;
	FixtureDef.shape = &Shape;
	//add to body
	body->CreateFixture(&FixtureDef);
	return body;
}

b2Body* CreatePhysicsBox(b2World& world, const bool dynamic, const RectangleShape rs) {
	return CreatePhysicsBox(world, dynamic, rs.getPosition(), rs.getSize());
}

void init() {
	const b2Vec2 gravity(0.0f, -10.0f);

	//construct a world whihc holds and simulates the physics bodies
	world = new b2World(gravity);

	//wall dimensions
	Vector2f walls[] = {
		//top
		Vector2f(gameWidth * .5f, 5.f), Vector2f(gameWidth, 10.f),
		//bottom
		Vector2f(gameWidth * .5f, gameHeight - 5.f), Vector2f(gameWidth, 10.f),
		//left
		Vector2f(5.f, gameHeight * .5f), Vector2f(10.f, gameHeight),
		//right
		Vector2f(gameWidth - 5.f, gameHeight * .5f), Vector2f(10.f, gameHeight)
	};

	//build walls
	for(int i = 0; i < 7; i += 2) {
		//create sfml shapes for each wall
		auto s = new RectangleShape();
		s->setPosition(walls[i]);
		s->setSize(walls[i + 1]);
		s->setOrigin(walls[i + 1].x / 2, walls[i + 1].y / 2);
		s->setFillColor(Color::White);
		sprites.push_back(s);
		//create a static physics body for the wall
		auto b = CreatePhysicsBox(*world, false, *s);
		bodies.push_back(b);
	}

	//create boxes
	for(int i = 1; i < 11; ++i) {
		//create sfml shapes for each box
		auto s = new RectangleShape();
		s->setPosition(Vector2f(i * (gameWidth / 12.f), gameHeight * .7f));
		s->setSize(Vector2f(50.f, 50.f));
		s->setOrigin(Vector2f(25.f, 25.f));
		s->setFillColor(Color::White);
		sprites.push_back(s);

		//create a dynamic physics body for the box
		auto b = CreatePhysicsBox(*world, true, *s);
		//give the box a spin
		b->ApplyAngularImpulse(5.f, true);
		bodies.push_back(b);
	}
}

void Update(RenderWindow &window) {
	static Clock clock;
	float dt = clock.restart().asSeconds();

	Event event;
	while(window.pollEvent(event)) {
		if(event.type == Event::Closed) {
			window.close();
			return;
		}
	}

	world->Step(dt, velocityIterations, positionIterations);

	for(int i = 0; i < bodies.size(); ++i) {
		//sync sprites to physics position
		sprites[i]->setPosition(invert_height(bv2_to_sv2(bodies[i]->GetPosition())));
		//sync sprites to physics rotation
		sprites[i]->setRotation((180 / b2_pi) * bodies[i]->GetAngle());
	}
}

void Render(RenderWindow &window) {
	for(auto &s : sprites) {
		window.draw(*s);
	}
}

int main()
{
	RenderWindow window(VideoMode(gameWidth, gameHeight), "Box2D");
	init();
	while (window.isOpen())
	{
		window.clear();
		Update(window);
		Render(window);
		window.display();
	}
	return 0;
}