#include <iostream>
#include <osmium/io/any_input.hpp>


bool wayToSort (const std::pair<std::string, long int>& lhs, const std::pair<std::string, long int>& rhs)
{
  return lhs.second < rhs.second;
}

int main(int argc, char * args[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << args[0] << " input.osmfile" << "\n";
        exit(1);
    }

    osmium::io::File osmfile(args[1]);
    osmium::io::Reader mReader(osmfile, osmium::osm_entity_bits::node); 

    std::vector< std::pair<std::string, long int> > nodeokID;
    std::vector<std::string> megallok;

    std::string buszTipus, jaratSzam, megalloNev, elozo;
    long int megalloID;

    while (osmium::memory::Buffer mBuffer = mReader.read())
    {
      for(auto& e : mBuffer)
      {
	osmium::Node& node (static_cast<osmium::Node&>(e));
	
	if(node.tags().get_value_by_key("name"))
	{
	  megalloNev = node.tags().get_value_by_key("name");
	  megalloID = node.id();
	  nodeokID.emplace_back(std::make_pair(megalloNev, megalloID));
	}
      }
    }
    std::sort(nodeokID.begin(), nodeokID.end(), wayToSort);
    
    mReader.close();
    
    osmium::io::Reader reader(osmfile, osmium::osm_entity_bits::relation);

    while (osmium::memory::Buffer mBuffer = reader.read())
    {
      for (auto& it : mBuffer)
      {
	osmium::Relation& mRelation (static_cast<osmium::Relation&>(it));

	if(mRelation.tags().get_value_by_key("route"))
	{
	  buszTipus = mRelation.tags().get_value_by_key("route");

	  if(buszTipus == "bus" || buszTipus == "trolleybus")
	  {
	    if(mRelation.tags().get_value_by_key("ref"))
	    {
	      jaratSzam = mRelation.tags().get_value_by_key("ref");
	      std::cout << jaratSzam << " - " << buszTipus << std::endl; 
	    }
	    else if(mRelation.tags().get_value_by_key("name"))
	    {
	      jaratSzam = mRelation.tags().get_value_by_key("name");
	      std::cout << jaratSzam << " - " << buszTipus << std::endl; 
	    }

	    for( const auto& e : mRelation.members())
	    {
	      for(auto& i : nodeokID)
	      {
		if(i.second == e.ref())
		{
		  if(elozo == i.first)
		    continue;
		  else
		  {
		    std::cout << "\t" << i.first << std::endl;
		    elozo = i.first;
		  }
		}
	      }
	    }
	  }
	}
      }
    }

    google::protobuf::ShutdownProtobufLibrary();
    reader.close();
    return 0;
}
