//#include <Banan2D.h>

#if 1

#include <Banan/ECS/Registry.h>

#include <chrono>
#include <iostream>

#define SIZE 1'000'000

struct TEST
{
	int i;
	void* ptr;

	TEST(int val)
	{
		i = val;

		ptr = new uint8_t[SIZE];
		memset(ptr, 0, SIZE);

		std::cout << (uint64_t)ptr << ' ';
		std::cout << "construction" << std::endl;
	}
	TEST(const TEST& other)
	{
		i = other.i;

		ptr = new uint8_t[SIZE];
		memcpy(ptr, other.ptr, SIZE);

		std::cout << (uint64_t)ptr << ' ';
		std::cout << "construction (copy)" << std::endl;
	}
	TEST(TEST&& other) noexcept
	{
		i = other.i;

		ptr = other.ptr;
		other.ptr = nullptr;

		std::cout << (uint64_t)ptr << ' ';
		std::cout << "construction (move)" << std::endl;
	}
	~TEST()
	{
		std::cout << (uint64_t)ptr << ' ';
		std::cout << "destruction" << std::endl;

		delete[] ptr;
	}

	TEST& operator=(const TEST& other)
	{
		i = other.i;

		ptr = new uint8_t[SIZE];
		memcpy(ptr, other.ptr, SIZE);

		std::cout << (uint64_t)ptr << ' ';
		std::cout << "assignment (copy)" << std::endl;

		return *this;
	}
	TEST& operator=(TEST&& other) noexcept
	{
		i = other.i;

		ptr = other.ptr;
		other.ptr = nullptr;

		std::cout << (uint64_t)ptr << ' ';
		std::cout << "assignment (move)" << std::endl;

		return *this;
	}
};

struct position
{
	position() : x(0), y(0) {}
	position(float x, float y) : x(x), y(y) {}
	float x;
	float y;
};

struct velocity
{
	velocity() : x(0), y(0) {}
	velocity(float x, float y) : x(x), y(y) {}
	float x;
	float y;
};

void ConstTest(const Banan::ECS::Registry& registry, Banan::ECS::Entity entity)
{
	{
		std::cout << "const TEST& from Get<TEST>" << std::endl;
		const TEST& crTEST = registry.Get<TEST>(entity);
		std::cout << crTEST.i << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "auto& [test, pos] from Get<TEST, position>" << std::endl;
		auto [test, pos] = registry.Get<TEST, position>(entity);
		std::cout << test.i << std::endl;
		std::cout << pos.x << ", " << pos.y << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "HasAllOf<TEST, position>" << std::endl;
		std::cout << (registry.HasAllOf<TEST, position>(entity) ? "true" : "false") << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "HasAllOf<TEST>" << std::endl;
		std::cout << (registry.HasAllOf<TEST>(entity) ? "true" : "false") << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "HasAllOf<position>" << std::endl;
		std::cout << (registry.HasAllOf<position>(entity) ? "true" : "false") << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "HasAllOf<TEST, int>" << std::endl;
		std::cout << (registry.HasAllOf<TEST, int>(entity) ? "true" : "false") << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "HasAllOf<position, int>" << std::endl;
		std::cout << (registry.HasAllOf<position, int>(entity) ? "true" : "false") << std::endl;
	}
}

int main(int argc, char** argv)
{
	Banan::ECS::Registry registry;
	
	auto entity1 = registry.Create();
	auto entity2 = registry.Create();
	auto entity3 = registry.Create();

	registry.Emplace<position>(entity1, 1.f, 0.3f);
	registry.Emplace<position>(entity2, 2.f, 0.2f);
	registry.Emplace<position>(entity3, 3.f, 0.1f);

	registry.Emplace<velocity>(entity1, .3f, .5f);
	registry.Emplace<velocity>(entity3, 1.f, .7f);

	{
		for (auto& pos : registry.View<position>())
			std::cout << pos.x << ", " << pos.y << std::endl;

		std::cout << std::endl;

		for (auto& vel : registry.View<velocity>())
			std::cout << vel.x << ", " << vel.y << std::endl;

		std::cout << std::endl;

		for (auto [pos, vel] : registry.View<position, velocity>())
		{
			std::cout << pos.x << ", " << pos.y << std::endl;
			std::cout << vel.x << ", " << vel.y << std::endl;
		}
	}

	std::cin.get();

	{
		std::cout << "entity 1:" << std::endl;

		bool hasPos = registry.HasAllOf<position>(entity1);
		std::cout << "  has position: " << (hasPos ? "true" : "false") << std::endl;
		if (hasPos)
		{
			const auto& [x, y] = registry.Get<position>(entity1);
			std::cout << "  " << x << ", " << y << std::endl;
		}

		bool hasVel = registry.HasAllOf<velocity>(entity1);
		std::cout << "  has velocity: " << (hasVel ? "true" : "false") << std::endl;
		if (hasVel)
		{
			const auto& [x, y] = registry.Get<velocity>(entity1);
			std::cout << "  " << x << ", " << y << std::endl;
		}
	}
	{
		std::cout << "entity 2:" << std::endl;

		bool hasPos = registry.HasAllOf<position>(entity2);
		std::cout << "  has position: " << (hasPos ? "true" : "false") << std::endl;
		if (hasPos)
		{
			const auto& [x, y] = registry.Get<position>(entity2);
			std::cout << "  " << x << ", " << y << std::endl;
		}

		bool hasVel = registry.HasAllOf<velocity>(entity2);
		std::cout << "  has velocity: " << (hasVel ? "true" : "false") << std::endl;
		if (hasVel)
		{
			const auto& [x, y] = registry.Get<velocity>(entity2);
			std::cout << "  " << x << ", " << y << std::endl;
		}
	}
	{
		std::cout << "entity 3:" << std::endl;

		bool hasPos = registry.HasAllOf<position>(entity3);
		std::cout << "  has position: " << (hasPos ? "true" : "false") << std::endl;
		if (hasPos)
		{
			const auto& [x, y] = registry.Get<position>(entity3);
			std::cout << "  " << x << ", " << y << std::endl;
		}

		bool hasVel = registry.HasAllOf<velocity>(entity3);
		std::cout << "  has velocity: " << (hasVel ? "true" : "false") << std::endl;
		if (hasVel)
		{
			const auto& [x, y] = registry.Get<velocity>(entity3);
			std::cout << "  " << x << ", " << y << std::endl;
		}
	}


	auto entity = registry.Create();
	
	{
		std::cout << "EMPLACE: (69)" << std::endl;
		registry.Emplace<TEST>(entity, 69);
		std::cout << registry.Get<TEST>(entity).i << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "EMPLACE (postion): { 0.5, 1.0 }" << std::endl;
		auto& pos = registry.Emplace<position>(entity);
		pos.x = 0.5f;
		pos.y = 1.0f;
	
		std::cout << registry.Get<position>(entity).x << ", " << registry.Get<position>(entity).y << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "PATCH: (420)" << std::endl;
		registry.Patch<TEST>(entity, [](auto& test) { test.i = 420; std::cout << (uint64_t)test.ptr << " patched" << std::endl; });
		std::cout << registry.Get<TEST>(entity).i << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "REPLACE: (-1)" << std::endl;
		registry.Replace<TEST>(entity, -1);
		std::cout << registry.Get<TEST>(entity).i << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "GET-REFERENCE: (404)" << std::endl;
		TEST& test = registry.Get<TEST>(entity);
		test.i = 404;
		std::cout << registry.Get<TEST>(entity).i << std::endl;
	}
	std::cout << std::endl;
	
	ConstTest(registry, entity);

	std::cout << std::endl;
	{
		std::cout << "REMOVE: (false)" << std::endl;
		registry.Remove<TEST>(entity);
		std::cout << (registry.HasAllOf<TEST>(entity) ? "true" : "false") << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "EMPLACE_OR_REPLACE (no prior)" << std::endl;
		registry.EmplaceOrReplace<TEST>(entity, 69);
		std::cout << registry.Get<TEST>(entity).i << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "EMPLACE_OR_REPLACE (has prior)" << std::endl;
		registry.EmplaceOrReplace<TEST>(entity, 420);
		std::cout << registry.Get<TEST>(entity).i << std::endl;
	}
	std::cout << std::endl;
	{
		std::cout << "DESTROY: (false)" << std::endl;
		registry.Destroy(entity);
		std::cout << (registry.Valid(entity) ? "true" : "false") << std::endl;
	}

	std::cin.get();
}

#else

#include <any>
#include <iostream>
#include <vector>

int main()
{
	std::any i_any(69);

	std::cout << *i_any._Cast<char>() << std::endl;

	std::cin.get();
}

#endif