class FrameTime
{

GLuint m_query;

public:
 FrameTime();
 void Init();
 void BeginCount();
 void EndCount();
};


void FrameTime::Init()
{
	glGenQueries(1,&m_query);
}

void FrameTime::Destroy()
{
      glDeleteQueries(1,&m_query);
}


