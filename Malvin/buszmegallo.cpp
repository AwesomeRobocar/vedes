#include <iostream>
#include <osmium/io/any_input.hpp>


int main(int argc, char * args[])
{
    //parancssori használat
    if (argc != 2)
    {
        std::cout << "Usage: " << args[0] << " input.osmfile" << "\n";
        exit(1);
    }

    osmium::io::File osmfile(args[1]); //a bemeneti fájl, ha nincs args, akkor std|in/out
    
    //////////////*Megállók*/////////////
    
    //beolvassa a bemeneti fájlt, ami egy osm típusú bitfield(fix nagyságú ugyanolyan tartalmú adathalmaz)
    osmium::io::Reader mReader(osmfile, osmium::osm_entity_bits::node); 

    //megcsináljuk a node-okat 1 vektorba, ami egy struktúrába rakja, ami 2 adott típusú objektumot vár(string, int most)
    std::vector< std::pair<std::string, long int> > nodeokID;
    std::vector<std::string> megallok;	//megállók neveit a megallok sztringbe

    std::string buszTipus, jaratSzam, megalloNev, elozo;
    long int megalloID;

    //amíg talál bemeneti adatot a fenti Reader interfészből, addig megy
    while (osmium::memory::Buffer mBuffer = mReader.read())
    {
      //végigmegy a Megállókon 
      //az új c++11-es for each szabvánnyal -> for(auto value : ints) ... 
      //azért használjuk ezt, mert nem akarunk változtatni a konténeren, ha akarnánk(módosítani, törölni) ott a sima for
      //auto-> nem kell megadni az e típusát, a c++ fordító megcsinálja helyetted, értünk van a drága
      for(auto& e : mBuffer)
      {
	osmium::Node& node = static_cast<osmium::Node&>(e);	//static_cast -> mert ez felfelé és lefelé is tud kasztolni
	
	//ha a nodenak van neve->megálló
	if(node.tags().get_value_by_key("name"))
	{
	  megalloNev = node.tags().get_value_by_key("name");
	  megalloID = node.id();
	  //bepakoljuk a vektorba a megálló nevét a node sorszámával 	make_pair->olyan típusúak lesznek, amiket kapott
	  nodeokID.emplace_back(std::make_pair(megalloNev, megalloID));
	}
      }
    }
    //sorbarendezi a megállókat
    std::sort(nodeokID.begin(), nodeokID.end(), [](const std::pair<std::string, long int>& lhs, const std::pair<std::string, long int>& rhs)
    {
        return lhs.second < rhs.second;
    });
    
    //befejezzük az olvasást, nem fontos, mert a konstruktor is megcsinálná, de kivételt is dobhat
    mReader.close();
    
    //////////////*Buszok*/////////////////

    osmium::io::Reader reader(osmfile, osmium::osm_entity_bits::relation);

    while (osmium::memory::Buffer mBuffer = reader.read())
    {
      for (auto& it : mBuffer)
      {
	osmium::Relation& mRelation = static_cast<osmium::Relation&>(it);

	//megnézzük, hogy milyen típussal van dolgunk
	if(mRelation.tags().get_value_by_key("route"))
	{
	  buszTipus = mRelation.tags().get_value_by_key("route");

	  if(buszTipus == "bus" || buszTipus == "trolleybus")
	  {
	    //számmal ellátott buszok
	    if(mRelation.tags().get_value_by_key("ref"))
	    {
	      jaratSzam = mRelation.tags().get_value_by_key("ref");
	      std::cout << jaratSzam << " - " << buszTipus << std::endl;
	      
	    }
	    //ha esetleg neve van
	    else if(mRelation.tags().get_value_by_key("name"))
	    {
	      jaratSzam = mRelation.tags().get_value_by_key("name");
	      std::cout << jaratSzam << " - " << buszTipus << std::endl;
	      
	    }

	    //végigmegy a megállókon a fenti Megállók pragma kiszámításából
            //az új c++11-es for each szabvánnyal -> for(auto value : ints) ... 
            //azért használjuk ezt, mert nem akarunk változtatni a konténeren, ha akarnánk(módosítani, törölni) ott a sima for
            //auto-> nem kell megadni az e típusát, a c++ megcsinálja helyetted
            for( auto& e : mRelation.members())
	    {
	      //az összes csomóponton
	      for(auto& i : nodeokID)
	      {
		//ha az adott csomópont buszmegálló
		if(i.second == e.ref())
		{
		  //ha az első megálló, mehet a for-each következő lépése
		  if(elozo == i.first)
		    continue;
		  //ha nem az első megálló, szépen kiírjuk azt
		  else
		  {
		    std::cout << "\t" << i.first;	//a neve
		    std::cout << lon << lat << std::endl;
		    elozo = i.first;	//továbblépünk
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    google::protobuf::ShutdownProtobufLibrary();
    //befejezzük az olvasást, nem fontos, mert a konstruktor is megcsinálná, de kivételt is dobhat
    reader.close();
    return 0;
}
