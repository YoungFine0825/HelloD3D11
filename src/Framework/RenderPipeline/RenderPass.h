#pragma once

namespace Framework 
{
	class RenderPass 
	{
	public:
		RenderPass();
		virtual ~RenderPass();
		void Invoke();
	protected:
		virtual void OnInvoke();
	private:

	};
}