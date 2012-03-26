#include "DbForum.h"
#include "ForumProxy.h"

int main()
{
	Forum* fmp = new DbForum(1);
	ForumProxy* fp = new ForumProxy(fmp);
	fp->set_forum_name("liwei");
}
