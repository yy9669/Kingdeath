
/*
 * StoryMode implements a story about The Planet of Choices.
 *
 */

#include "Mode.hpp"

struct StoryMode : Mode {
	StoryMode();
	virtual ~StoryMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//called to create menu for current scene:
	void enter_scene();

	//------ story state -------
	enum {
		Foltest,
		Death,
		Shadow,
		Guard,
		Bed,
	} location = Foltest;
	bool have_guard = false;
	bool drink_wine = false;
	struct {
		bool first_visit = true;
	} foltest;
	struct {
		int death_reason = 0;
	} death;

	struct {
		bool first_visit = true;
	} shadow;

	struct {
		bool first_visit = true;
	} guard;

	struct {
		bool first_visit = true;
	} bed;

	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(256, 224);
};
