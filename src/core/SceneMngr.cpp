#include <UEngine/SceneMngr.h>

using namespace Ubpa;

void SceneMngr::Insert(Scene* scene) {
	scenes->insert(scene);
}

void SceneMngr::Erase(Scene* scene) {
	scenes->erase(scene);
}

void SceneMngr::Active(Scene* scene, SObj* main_camera_sobj) {
	scenes->insert(scene);
	actived_scene = scene;
	this->main_camera_sobj = main_camera_sobj;
}
