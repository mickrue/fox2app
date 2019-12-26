# fox2app
This project can be used to convert the exported firefighter schedule in Fox-112 format, into the XML format used by the Android App

I'm a voluntary fire-fighter at the fire-station of my village. Just for fun I started to create an Android App that my other 
fire-fighter buddies can install on their Android phones to get a notification when they have a training session coming up, where
they are expected to attend.
To be able to show the anual schedule in the App, I created an XML format the App can process and is able to differentiate different 
types of practice sessions, like radio operation training, truck driving, technical rescue practice, figthing open fire and the like

Many languages would do the job, but I chose C++ using the Qt application framework as I know this the best. I failed to use Python 
as the exported CSV file with the anual schedule is a character encoding mess and Qt was able to deal the best with it.  
