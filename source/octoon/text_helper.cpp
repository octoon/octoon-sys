#include <octoon/text_helper.h>
#include <octoon/text_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>

#include <octoon/model/text_meshing.h>

namespace octoon
{
	GameObjectPtr
	TextHelper::create(const wchar_t* text, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeMesh(model::makeTextContours(text, { fontPath, fontsize })));
		object->addComponent<MeshRendererComponent>(std::make_shared<material::Material>());

		return object;
	}

	GameObjectPtr
	TextHelper::create(const char* u8str, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<TextComponent>(u8str)->setTextMeshing(std::make_shared<model::TextMeshing>(fontPath, fontsize));
		object->addComponent<MeshRendererComponent>(std::make_shared<material::Material>());

		return object;
	}
}