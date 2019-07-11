#ifndef OCTOON_ANIMATION_COMPONENT_H_
#define OCTOON_ANIMATION_COMPONENT_H_

#include <octoon/animation/animation.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT AnimationComponent : public GameComponent
	{
		OctoonDeclareSubClass(AnimationComponent, GameComponent)
	public:
		AnimationComponent() noexcept;
		explicit AnimationComponent(animation::AnimationClip<float>&& clips) noexcept;
		explicit AnimationComponent(animation::AnimationClips<float>&& clips) noexcept;
		explicit AnimationComponent(const animation::AnimationClip<float>& clips) noexcept;
		explicit AnimationComponent(const animation::AnimationClips<float>& clips) noexcept;
		virtual ~AnimationComponent() noexcept;

		void play() noexcept;
		void pause() noexcept;
		void stop() noexcept;

		virtual GameComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() except override;
		virtual void onDeactivate() noexcept override;

	private:
		AnimationComponent(const AnimationComponent&) = delete;
		AnimationComponent& operator=(const AnimationComponent&) = delete;

	private:
		animation::AnimationClips<float> clips_;
	};
}

#endif