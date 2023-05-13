
#include <unordered_map>

#include "Resource.h"

namespace Framework 
{
	namespace Resource 
	{
		ResourceGUID m_next_resouce_guid = 1;

		typedef std::unordered_map<ResourceGUID, IResource*> ResourcesMap;
		ResourcesMap m_resMap;

		IResource::IResource() 
		{
			m_id = m_next_resouce_guid;
			m_next_resouce_guid++;
			//
			m_resMap[m_id] = this;
		}

		IResource::~IResource() 
		{
			
		}

		void IResource::Release() 
		{
			ResourcesMap::iterator it = m_resMap.find(m_id);
			if (it != m_resMap.end()) 
			{
				m_resMap.erase(it);
			}
		}

		IResource* Find(ResourceGUID guid) 
		{
			return nullptr;
		}

		bool ReleaseWithGUID(ResourceGUID guid)
		{
			ResourcesMap::iterator it = m_resMap.find(guid);
			if (it == m_resMap.end()) 
			{
				return false;
			}
			IResource* res = it->second;
			m_resMap.erase(it);
			res->Release();
			delete res;
			return true;
		}
	}
}