#pragma once

namespace Utilites
{

class NonCopyable
{
protected:
	NonCopyable() { /**/ }

private:
	NonCopyable(const NonCopyable&) { /**/ }
	NonCopyable(NonCopyable&&) { /**/ }
	NonCopyable& operator= (const NonCopyable&) { /**/ }
};

} //namespace