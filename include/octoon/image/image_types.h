#ifndef OCTOON_IMAGE_TYPES_H_
#define OCTOON_IMAGE_TYPES_H_

#include <memory>
#include <iostream>

#include <octoon/image/image_def.h>
#include <octoon/image/image_format.h>

namespace octoon
{
	namespace image
	{
		enum class swizzle_t : uint8_t
		{
			R,
			RG,
			RGB,
			BGR,
			RGBA,
			BGRA,
			ABGR,
			Depth,
			Stencil,
			DepthStencil,
			BeginRange = R,
			EndRange = DepthStencil,
			RangeSize = (EndRange - BeginRange + 1),
		};

		enum class value_t : std::uint8_t
		{
			Null,
			SNorm,
			UNorm,
			UNorm5_6_5,
			UNorm5_5_5_1,
			UNorm1_5_5_5,
			UNorm2_10_10_10,
			UFloatB10G11R11Pack32,
			UFloatE5B9G9R9Pack32,
			SScaled,
			UScaled,
			SInt,
			UInt,
			SRGB,
			Float,
			D16UNorm_S8UInt,
			D24UNorm_S8UInt,
			D24UNormPack32,
			D32_SFLOAT_S8UInt,
			Compressed,
			BeginRange = SNorm,
			EndRange = Compressed,
			RangeSize = (EndRange - BeginRange + 1),
		};

		typedef std::shared_ptr<class Image> ImagePtr;
		typedef std::shared_ptr<class ImageHandler> ImageHandlerPtr;

		using istream = std::istream;
		using ostream = std::ostream;
	}
}

#endif