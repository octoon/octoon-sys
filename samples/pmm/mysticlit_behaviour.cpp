#include "mysticlit_behaviour.h"

namespace MysticLit
{
	OctoonImplementSubClass(MysticlitBehaviour, GameComponent, "MysticlitBehaviour")

	MysticlitBehaviour::MysticlitBehaviour() noexcept
	{
	}

	MysticlitBehaviour::~MysticlitBehaviour() noexcept
	{
	}

	void
	MysticlitBehaviour::addComponent(IMysticLitComponent* component) noexcept
	{
		components_.push_back(component);
	}

	void
	MysticlitBehaviour::removeComponent(const IMysticLitComponent* component) noexcept
	{
		auto it = std::find(components_.begin(), components_.end(), component);
		if (it != components_.end())
			components_.erase(it);
	}

	IMysticLitComponent*
	MysticlitBehaviour::getComponent(const std::type_info& type) const noexcept
	{
		for (auto& it : components_)
		{
			if (it->type_info() == type)
				return it;
		}

		return nullptr;
	}

	void
	MysticlitBehaviour::onActivate() noexcept
	{
		profile_ = std::make_unique<MysticLitProfile>();

		context_ = std::make_shared<MysticLitContext>();
		context_->behaviour = this;
		context_->profile = profile_.get();

		fileComponent_ = std::make_unique<FileComponent>();
		entitiesComponent_ = std::make_unique<EntitiesComponent>();
		offlineComponent_ = std::make_unique<OfflineComponent>();
		playerComponent_ = std::make_unique<PlayerComponent>();
		h264Component_ = std::make_unique<H264Component>();

		fileComponent_->init(context_, profile_->fileModule);
		entitiesComponent_->init(context_, profile_->entitiesModule);
		offlineComponent_->init(context_, profile_->offlineModule);
		playerComponent_->init(context_, profile_->timeModule);
		h264Component_->init(context_, profile_->h264Module);

		this->addComponent(fileComponent_.get());
		this->addComponent(entitiesComponent_.get());
		this->addComponent(offlineComponent_.get());
		this->addComponent(playerComponent_.get());
		this->addComponent(h264Component_.get());

		this->addComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		this->addMessageListener("editor:menu:file:open", std::bind(&MysticlitBehaviour::onOpenProject, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:file:save", std::bind(&MysticlitBehaviour::onSaveProject, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:file:saveAs", std::bind(&MysticlitBehaviour::onSaveAsProject, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:file:import", std::bind(&MysticlitBehaviour::onOpenModel, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:file:export", std::bind(&MysticlitBehaviour::onSaveModel, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:file:exit", std::bind(&MysticlitBehaviour::exit, this, std::placeholders::_1));	
		this->addMessageListener("editor:menu:file:picture", std::bind(&MysticlitBehaviour::onRenderPicture, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:file:video", std::bind(&MysticlitBehaviour::onRenderVideo, this, std::placeholders::_1));

		this->addMessageListener("editor:menu:setting:render", std::bind(&MysticlitBehaviour::play, this, std::placeholders::_1));
		this->addMessageListener("editor:menu:setting:mode", std::bind(&MysticlitBehaviour::offlineMode, this, std::placeholders::_1));

		this->getFeature<octoon::GameBaseFeature>()->getGameObjectManager()->addMessageListener("feature:input:drop", std::bind(&MysticlitBehaviour::onFileDrop, this, std::placeholders::_1));
	}

	void
	MysticlitBehaviour::onDeactivate() noexcept
	{
		fileComponent_.reset();
		entitiesComponent_.reset();
		offlineComponent_.reset();

		context_.reset();
		profile_.reset();

		this->removeComponentDispatch(octoon::GameDispatchType::FixedUpdate);
		this->removeMessageListener("editor:menu:file:open", std::bind(&MysticlitBehaviour::onOpenProject, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:file:save", std::bind(&MysticlitBehaviour::onSaveProject, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:file:saveAs", std::bind(&MysticlitBehaviour::onSaveAsProject, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:file:import", std::bind(&MysticlitBehaviour::onOpenModel, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:file:export", std::bind(&MysticlitBehaviour::onSaveModel, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:file:picture", std::bind(&MysticlitBehaviour::onRenderPicture, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:file:video", std::bind(&MysticlitBehaviour::onRenderVideo, this, std::placeholders::_1));

		this->removeMessageListener("editor:menu:setting:render", std::bind(&MysticlitBehaviour::play, this, std::placeholders::_1));
		this->removeMessageListener("editor:menu:setting:mode", std::bind(&MysticlitBehaviour::offlineMode, this, std::placeholders::_1));

		this->getFeature<octoon::GameBaseFeature>()->getGameObjectManager()->removeMessageListener("feature:input:drop", std::bind(&MysticlitBehaviour::onFileDrop, this, std::placeholders::_1));
	}

	void
	MysticlitBehaviour::onFixedUpdate() noexcept
	{
		if (h264Component_->getActive())
			h264Component_->update();
	}

	void
	MysticlitBehaviour::onFileDrop(const octoon::runtime::any& data) noexcept
	{
		if (data.type() == typeid(std::vector<const char*>))
		{
			auto files = octoon::runtime::any_cast<std::vector<const char*>>(data);
			if (files.empty())
				return;

			std::string_view str(files.front());
			auto ext = str.substr(str.find_first_of("."));
			if (ext == ".pmm")
				fileComponent_->open(std::string(str));
		}
	}

	void
	MysticlitBehaviour::onOpenProject(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileOpenBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->projectExtensions[0]))
			return;

		fileComponent_->open(filepath.data());
	}

	void
	MysticlitBehaviour::onSaveProject(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->projectExtensions[0]))
			return;

		fileComponent_->open(filepath.data());
	}

	void
	MysticlitBehaviour::onSaveAsProject(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->projectExtensions[0]))
			return;
	}

	void
	MysticlitBehaviour::onOpenModel(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileOpenBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->modelExtensions[0]))
			return;

		fileComponent_->importModel(filepath.data());
	}

	void
	MysticlitBehaviour::onSaveModel(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->modelExtensions[0]))
			return;
	}

	void
	MysticlitBehaviour::onRenderVideo(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileSaveBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->videoExtensions[0]))
			return;

		playerComponent_->play();
		entitiesComponent_->play();
		h264Component_->play();
	}

	void
	MysticlitBehaviour::onRenderPicture(const octoon::runtime::any& data) noexcept
	{
		auto pathLimits = fileComponent_->getModel()->PATHLIMIT;
		std::vector<std::string::value_type> filepath(pathLimits);
		if (!fileComponent_->showFileOpenBrowse(filepath.data(), pathLimits, fileComponent_->getModel()->imageExtensions[0]))
			return;
	}

	void 
	MysticlitBehaviour::exit(const octoon::runtime::any& data) noexcept
	{
		std::exit(0);
	}

	void
	MysticlitBehaviour::play(const octoon::runtime::any& data) noexcept
	{
		assert(data.type() == typeid(bool));

		auto play = octoon::runtime::any_cast<bool>(data);
		if (play)
		{
			playerComponent_->play();
			entitiesComponent_->play();
		}
		else
		{
			playerComponent_->stop();
			entitiesComponent_->stop();
		}
	}

	void
	MysticlitBehaviour::offlineMode(const octoon::runtime::any& data) noexcept
	{
		assert(data.type() == typeid(bool));
		offlineComponent_->setActive(octoon::runtime::any_cast<bool>(data));
	}

	octoon::GameComponentPtr
	MysticlitBehaviour::clone() const noexcept
	{
		return std::make_shared<MysticlitBehaviour>();
	}
}