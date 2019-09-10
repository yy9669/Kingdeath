#include "StoryMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "MenuMode.hpp"

Sprite const *sprite_left_select = nullptr;
Sprite const *sprite_right_select = nullptr;

Sprite const *sprite_foltest = nullptr;
Sprite const *sprite_death = nullptr;
Sprite const *sprite_shadow = nullptr;
Sprite const *sprite_guard = nullptr;
Sprite const *sprite_bed = nullptr;
Sprite const *sprite_wine = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("king-death"));
	sprite_death = &ret->lookup("death");
	sprite_foltest = &ret->lookup("foltest");
	sprite_shadow = &ret->lookup("shadow");
	sprite_guard = &ret->lookup("guard");
	sprite_bed = &ret->lookup("bed");
	sprite_wine = &ret->lookup("wine");
	return ret;
});

StoryMode::StoryMode() {
}

StoryMode::~StoryMode() {
}

bool StoryMode::handle_event(SDL_Event const &, glm::uvec2 const &window_size) {
	if (Mode::current.get() != this) return false;

	return false;
}

void StoryMode::update(float elapsed) {
	if (Mode::current.get() == this) {
		//there is no menu displayed! Make one:
		enter_scene();
	}
}

void StoryMode::enter_scene() {
	//just entered this scene, adjust flags and build menu as appropriate:
	std::vector< MenuMode::Item > items;
	glm::vec2 at(20.0f, view_max.y - 30.0f);
	auto add_text = [&items,&at](std::string text) {
		items.emplace_back(text, nullptr, 0.8f, nullptr, at);
		at.y -= 20.0f;
	};

	auto add_choice = [&items,&at](std::string text, std::function< void(MenuMode::Item const &) > const &fn) {
		items.emplace_back(text, nullptr, 0.8f, fn, at + glm::vec2(8.0f, 0.0f));
		at.y -= 20.0f;
	};

	if (location == Foltest) {
		if (foltest.first_visit) {
			foltest.first_visit = false;
			add_text("Foltest is the King of Temeria. ");
			add_text("He is very powerful, but many spies want to kill him.");
		} else {
			//add_text(text_dunes_return);
			add_text("Foltest comes to his castle.");
		}
		at.y -= 8.0f; //gap before choices
		add_choice("Go to palace", [this](MenuMode::Item const &){
			location = Shadow;
			Mode::current = shared_from_this();
		});
		add_choice("Summon some guards", [this](MenuMode::Item const &){
			location = Guard;
			have_guard = true;
			Mode::current = shared_from_this();
		});
	} else if (location == Shadow) {
		if (shadow.first_visit) {
			add_text("There is a black shadow behind the window.");
			shadow.first_visit=false;
			if (have_guard) {
				add_text("But Foltest has guards with him.");
			}
		}
		else {
			add_text("There is still a black shadow behind the window.");
			if (have_guard) {
				add_text("But Foltest has guards with him.");
			}
		}

		at.y -= 8.0f; //gap before choices
		add_choice("Go to bed chamber", [this](MenuMode::Item const &){
			if (have_guard) {
				location = Bed;
				Mode::current = shared_from_this();
			}
			else{	
				death.death_reason=0;
				location = Death;
				Mode::current = shared_from_this();			
			}
		});
		add_choice("Go to castle", [this](MenuMode::Item const &){
			location = Foltest;
			Mode::current = shared_from_this();
		});
	} else if (location == Guard) {
		if (guard.first_visit) {
			add_text("Foltest finds some guards.");
			guard.first_visit=false;
		} else {
			add_text("More guards come to protect Foltest.");
		}
		add_choice("Go to castle", [this](MenuMode::Item const &){
			location = Foltest;
			Mode::current = shared_from_this();
		});
	} else if (location == Bed) {
		add_text("This is Foltest's bedroom. ");
		if (bed.first_visit){
			add_text("Would you like to drink some wine before go to sleep ?");
		}
		add_choice("Go to sleep", [this](MenuMode::Item const &){
			location = Bed;
			Mode::current = shared_from_this();
		});
		add_choice("Drink some wine", [this](MenuMode::Item const &){
			location = Death;
			death.death_reason=1;
			Mode::current = shared_from_this();
		});
		add_choice("Go to castle", [this](MenuMode::Item const &){
			location = Foltest;
			Mode::current = shared_from_this();
		});		
	} else if (location == Death) {
		add_text("Game over, Foltest died");
		if(death.death_reason==1){
			add_text("because of a glass of poisonous wine");
		}
		else if(death.death_reason==0){
			add_text("because of an assassin in the bed chamber");
		}
		add_choice("Play again", [this](MenuMode::Item const &){
			location = Foltest;
			drink_wine=false;
			have_guard=false;
			foltest.first_visit=true;
			shadow.first_visit=true;
			guard.first_visit=true;
			bed.first_visit=true;
			Mode::current = shared_from_this();
		});			
	} 
	std::shared_ptr< MenuMode > menu = std::make_shared< MenuMode >(items);
	menu->atlas2 = sprites;
	menu->atlas=  new SpriteAtlas(data_path("the-planet"));
	menu->left_select = sprite_left_select;
	menu->right_select = sprite_right_select;
	menu->view_min = view_min;
	menu->view_max = view_max;
	menu->background = shared_from_this();
	Mode::current = menu;
}

void StoryMode::draw(glm::uvec2 const &drawable_size) {
	//clear the color buffer:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);
		glm::vec2 ul = glm::vec2(view_min.x, view_max.y);
		if (location == Foltest) {
			draw.draw(*sprite_foltest, ul);
		} else if (location == Shadow) {
			draw.draw(*sprite_shadow, ul);
		} else if (location == Guard) {
			draw.draw(*sprite_guard, ul);
		}else if (location == Death) {
			draw.draw(*sprite_death, ul);
		}else if (location == Bed) {
			draw.draw(*sprite_bed, ul);
			if(bed.first_visit){
				draw.draw(*sprite_wine, ul);
				bed.first_visit=false;				
			}
		}
	}
	GL_ERRORS(); //did the DrawSprites do something wrong?
}
