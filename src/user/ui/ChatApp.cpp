// user/ui/ChatApp.cpp

#include "core/ui/Widgets.hpp"
#include "user/ui/Widgets.hpp"

#include<array>
#include<map>

static const std::array s_ChatMessages =
{
	ChatEntry{ "Luke", "Hey there! How was your day?" },
	ChatEntry{ "Jane", "I've had a terrible day." },
	ChatEntry{ "Luke", "That's terrible!" },
	ChatEntry{ "Luke", "What happened?" },
	ChatEntry{ "Jane", "It all started when I forgot my keys as I left for work." },
	ChatEntry{ "Jane", "I feel so silly because I should have remembered them when I couldn't start the car." },
	ChatEntry{ "Luke", "Oh dear - what did you do?" },
	ChatEntry{ "Luke", "Wait - how did you start the car without your keys??" },
	ChatEntry{ "Jane", "That's just the thing, it started all by itself." },
	ChatEntry{ "Luke", "What?? That's crazy. You're pulling my leg." },
	ChatEntry{ "Jane", "I speak sincerely." }
};

static const std::map<std::string_view, rgba32> s_ChatColours =
{
	{ "Luke", rgba32{ 176,  0,  81, 255 } },
	{ "Jane", rgba32{ 145, 39, 143, 255 } }
};

Key ChatApp()
{
  return Column( AutoWidth, AutoHeight,
	{
		RoundedBox( AutoWidth, AutoHeight, RoundedBoxFormat{ .colour = Blue, .radius = 10 }, DefaultHitTest, Text( AutoWidth, AutoHeight, "Hello", Font{} ) ),
		Padding( AutoWidth, AutoHeight, 5, 5, 10, 10,
		
			// child
			VScrollBox(
			
				// width, height
				AutoWidth, AutoHeight,

				// initialVScroll
				0,

				// custom
				// add above chat entries after default initState has been run
				Custom
				{
					.extraInitState = [] ( Key self )
					{
						// for( auto i = 0; i < 1; ++i )
						for( auto const& message : s_ChatMessages )
						{
							// auto const& message = s_ChatMessages.at( i );
							auto chatBubbles = ChatBubbles(
								WidthAtLeast( 0.5f ), AutoHeight,
								s_ChatColours.at( message.author ),
								message
							);
							CreateChildWidget(
								self,
								Padding( AutoWidth, AutoHeight, 5, 5, 5, 5,

									// child
									message.author == "Jane"
									?	AlignLeft(
											AutoWidth, AutoHeight,
											chatBubbles		
										)
									: AlignRight(
											AutoWidth, AutoHeight,
											chatBubbles
										)
								)
							);
						}
					}
				},

				// children
				{}
			)
		)
	}
	);
}