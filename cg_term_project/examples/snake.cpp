#include "snake.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>

namespace examples::snake {

	void run_agent_ai(ecs::Writable<Snake> snake, ecs::EntitiesWith<MealCell> meal_cells) {
		if (snake->dead) return;
		if (meal_cells.empty()) return;
		auto& [id, meal_cell] = *meal_cells.cbegin();
		auto dx = meal_cell->x - snake->x;
		auto dy = meal_cell->y - snake->y;

		if (dx > 0 && snake->change_dir(DirectionValue::right)) return;
		if (dx < 0 && snake->change_dir(DirectionValue::left)) return;
		if (dy > 0 && snake->change_dir(DirectionValue::down)) return;
		if (dy < 0 && snake->change_dir(DirectionValue::up)) return;

		if (snake->change_dir(DirectionValue::right)) return;
		if (snake->change_dir(DirectionValue::left)) return;
		if (snake->change_dir(DirectionValue::down)) return;
		if (snake->change_dir(DirectionValue::up)) return;
	}
	void move_snake(ecs::Writable<Snake> snake, ecs::EntityApi entity_api) {
		if (snake->dead) return;
		int dx = 0, dy = 0;
		switch (snake->dir) {
		case DirectionValue::left:
			dx = -1;
			break;
		case DirectionValue::right:
			dx = +1;
			break;
		case DirectionValue::up:
			dy = -1;
			break;
		case DirectionValue::down:
			dy = +1;
			break;
		}
		if (dx == 0 && dy == 0) return;
		entity_api.spawn().with(SnakeTrailCell{ .x{snake->x},.y{snake->y} });
		snake->x += dx;
		snake->y += dy;
	}
	void kill_snake(ecs::Writable<Snake> snake, Board board, ecs::EntitiesWith<SnakeTrailCell> snake_trail_cells) {
		bool dead = ([&] {
			if (0 > snake->x || snake->x >= board.width) return true;
			if (0 > snake->y || snake->y >= board.height) return true;

			for (auto& [id, snake_trail_cell] : snake_trail_cells) {
				if (snake_trail_cell->x == snake->x && snake_trail_cell->y == snake->y) return true;
			}
			return false;
			}
		)();

		if (dead) {
			snake->dead = true;
			return;
		}
	}
	void age_trail_cells(Snake snake, ecs::EntitiesWithWritable<SnakeTrailCell> snake_trail_cells, ecs::EntityApi entity_api) {
		if (snake.dead) return;
		for (auto& [id, snake_trail_cell] : snake_trail_cells) {
			snake_trail_cell->age++;
			if (snake_trail_cell->age >= snake.length) {
				entity_api.remove(id);
			}
		}
	}
	void consume_meal(ecs::Writable<Snake> snake, ecs::EntitiesWith<MealCell> meal_cells, ecs::EntityApi entity_api) {
		for (auto& [id, meal_cell] : meal_cells) {
			if (snake->x == meal_cell->x && snake->y == meal_cell->y) {
				snake->length++;
				entity_api.remove(id);
				return;
			}
		}
	}
	void repop_meal(Snake snake, Board board, ecs::EntitiesWith<SnakeTrailCell> snake_trail_cells, ecs::EntitiesWith<MealCell> meal_cells, ecs::EntityApi entity_api) {
		if (!meal_cells.empty()) return;

		std::set<std::pair<int, int>> empties;
		for (int y = 0;y < board.height;y++) {
			for (int x = 0;x < board.width;x++) {
				empties.emplace(x, y);
			}
		}
		auto occupy = [&](int x, int y) {empties.erase(std::make_pair(x, y));};

		occupy(snake.x, snake.y);
		for (auto& [id, cell] : snake_trail_cells) {
			occupy(cell->x, cell->y);
		}
		if (empties.empty()) return;
		static std::mt19937 generator(std::random_device{}());
		size_t n = std::uniform_int_distribution<size_t>(0, empties.size() - 1)(generator);
		auto iter = empties.begin();
		for (size_t i = 0;i < n;i++) iter++;
		auto& coords = *iter;

		entity_api.spawn().with(MealCell{ coords.first,coords.second });

	}
	void update_elapsed(ecs::Writable<Elapsed> elapsed) {
		elapsed->ticks++;
	}
	void reset_on_game_over(Board board, ecs::Writable<Snake> snake, ecs::EntitiesWith<MealCell> meal_cells, ecs::EntitiesWith<SnakeTrailCell> snake_trail_cells, ecs::EntityApi entity_api) {
		if (!snake->dead) return;
		snake->dead = false;
		snake->dir = DirectionValue::none;
		snake->highscore = std::max(snake->highscore, snake->length);
		snake->length = 1;
		snake->x = board.width / 2;
		snake->y = board.height / 2;
		for (auto& [id, _] : meal_cells) {
			entity_api.remove(id);
		}
		for (auto& [id, _] : snake_trail_cells) {
			entity_api.remove(id);
		}
	}

	void print_board(Cells cells, Snake snake, Elapsed elapsed, Board board, ecs::EntitiesWith<MealCell> meal_cells, ecs::EntitiesWith<SnakeTrailCell> snake_trail_cells) {
		if (!snake.dead) return;
		auto get_type = [&](size_t x, size_t y) { return cells.types[y * board.width + x];};

		std::stringstream ss;

		ss << "#ticks: " << elapsed.ticks << '\n';
		ss << "highscore: " << snake.highscore << '\n';
		for (int y = 0;y < board.height;y++) {
			for (int x = 0;x < board.width;x++) {
				switch (cells.types[y * board.width + x]) {
				case Cells::Type::empty:
					ss << '.';
					break;
				case Cells::Type::meal:
					ss << '*';
					break;
				case Cells::Type::trail:
					ss << 'o';
					break;
				case Cells::Type::head:
					if (snake.dead)
					{
						ss << 'x';
						break;
					}
					switch (snake.dir) {
					case DirectionValue::down:
						ss << 'v';
						break;
					case DirectionValue::up:
						ss << 'n';
						break;
					case DirectionValue::left:
						ss << '<';
						break;
					case DirectionValue::right:
						ss << '>';
						break;
					default:
						ss << 'o';
						break;
					}
					break;
				}
			}
			ss << '\n';
		}
		std::cout << ss.str();
	}

	void update_cells(ecs::Writable<Cells> cells, Snake snake, Board board, ecs::EntitiesWith<MealCell> meal_cells, ecs::EntitiesWith<SnakeTrailCell> snake_trail_cells) {
		if (snake.dead) return;
		cells->types.clear();
		cells->types.assign(board.width * board.height, Cells::Type::empty);

		auto set_type = [&](size_t x, size_t y, Cells::Type type) {
			if (cells->types.size() <= y * board.width + x) return;
			cells->types[y * board.width + x] = type;
			};


		for (auto& [id, meal_cell] : meal_cells) {
			set_type(meal_cell->x, meal_cell->y, Cells::Type::meal);
		}

		for (auto& [id, snake_trail_cell] : snake_trail_cells) {
			set_type(snake_trail_cell->x, snake_trail_cell->y, Cells::Type::trail);
		}
		set_type(snake.x, snake.y, Cells::Type::head);
	}

	ecs::World create_world() {
		auto world = ecs::World();
		world.manage_resource(Elapsed{});
		world.manage_resource(Board{});
		world.manage_resource(Cells{});
		world.manage_resource(Snake{});

		world.register_component<SnakeTrailCell>();
		world.register_component<MealCell>();

		return world;
	}

	ecs::SystemForest create_system_forest(ecs::World* world) {
		auto system_forest = ecs::SystemForest(world);

		system_forest.add(update_elapsed)
			.followed_by(move_snake)
			.followed_by(consume_meal)
			.followed_by(age_trail_cells)
			.followed_by(kill_snake)
			.followed_by(repop_meal)
			.followed_by(update_cells)
			.followed_by(print_board)
			.followed_by(run_agent_ai)
			.followed_by(reset_on_game_over);

		return system_forest;
	}

	bool Snake::change_dir(DirectionValue dir_cand) {
		auto dir_backward = ([&] {
			switch (dir) {
			case DirectionValue::left:
				return DirectionValue::right;
			case DirectionValue::right:
				return DirectionValue::left;
			case DirectionValue::down:
				return DirectionValue::up;
			case DirectionValue::up:
				return DirectionValue::down;
			default:
				return DirectionValue::none;
			}
			}
		)();
		if (dir_cand == dir_backward) return false;
		dir = dir_cand;
		return true;
	}

}