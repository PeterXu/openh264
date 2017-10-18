H264PRS_SRCDIR=codec/console/prs
H264PRS_CPP_SRCS=\
	$(H264PRS_SRCDIR)/src/h264prs.cpp

H264PRS_OBJS += $(H264PRS_CPP_SRCS:.cpp=.$(OBJ))

OBJS += $(H264PRS_OBJS)

$(H264PRS_SRCDIR)/%.$(OBJ): $(H264PRS_SRCDIR)/%.cpp
	$(QUIET_CXX)$(CXX) $(CFLAGS) $(CXXFLAGS) $(INCLUDES) $(PARSER_CFLAGS) $(PARSER_INCLUDES) -I. -c $(CXX_O) $<

h264prs$(EXEEXT): $(H264PRS_OBJS) $(H264PRS_DEPS)
	$(QUIET_CXX)$(CXX) $(CXX_LINK_O) $(H264PRS_OBJS) -lparser $(PARSER_LDFLAGS) $(LDFLAGS)

binaries: h264prs$(EXEEXT)
BINARIES += h264prs$(EXEEXT)
