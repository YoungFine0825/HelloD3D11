#pragma once

#include <string>

namespace Framework 
{
	namespace Resource 
	{
		enum Resource_Type {
			RESOURCE_TYPE_MESH,
			RESOURCE_TYPE_SHADER,
			RESOURCE_TYPE_TEXTURE,
			RESOURCE_TYPE_MODEL
		};

		typedef size_t ResourceGUID;
		typedef std::string ResourceUrl;

		class IResource
		{
		public:
			IResource();
			virtual ~IResource();
			//
			virtual void Release();
			//
			Resource_Type GetType() { return m_type; };
			ResourceGUID GetGUID() { return m_id; };
			//
			ResourceUrl GetUrl() { return m_url; }
			IResource* SetUrl(ResourceUrl url) { m_url = url; return this; }
		protected:
			ResourceUrl m_url;
			Resource_Type m_type;
			ResourceGUID m_id;
			bool m_isLoaded = false;
		};

		IResource* Find(ResourceGUID guid);
		bool ReleaseWithGUID(ResourceGUID guid);
	}

}