#include <cstdint>
const unsigned short FREQ_BINS = 50;
const unsigned short TIMESLICES = 14;

//these width and length of the bins is hard coded now, but will need to be pseudo dynamic.  I will probably
//need to have XbyY defined for multiple kinds here based on the settings of the receiver...


//what i think is important here is knowing that 
//the antenna doesnt know about az/el/etc of the transmission - you would have to lower your apeture, which would
//be controlled by the world view of this player, so this structure would still work.
//what you need to do is look for the frequency and pulsing that you know you sent out to be received back in this form
#pragma pack(push, 1) //don't pad bytes between structure members
struct CPR_Packet {
    uint32_t seq; //_t forces fixed width to expected size regarless of hardware
    uint64_t timestamp_ns;
    float freqStart;
    float freqEnd;
    float timeStart;
    float timeEnd;
    //float    value;
    //values from old
    //float frequency;        // GHz
    //float power;            // dBm
    //float azimuth;          // Degrees
    //float elevation;        // Degrees
    short pulsesPerBox;       // count

    /*I may need to have an on/off pulse mapper for each timeslice, which would take away some of my packet space*/

    //float prf;              // Hz
    //float originX;
    //float originY;
    //float originZ;
    //float originYaw;
    //float originPitch;
    //float originRoll;
    //end old values
    //what i actually want is the hash of freqs and time blocks they fit in, with power values in the blocks
    unsigned short freqbinByTimeslicePower[FREQ_BINS][TIMESLICES];
};
#pragma pack(pop)

//this is here to ensure performance.  There are a couple of options here, but overall we need to maintain one packet per message, or our throughput will be cut in half.  
//By default, the payload size will be 1472 bytes.  we can increase this both at the network switch and OS level, but would require "point of install" knowledge and setup
//to handle correctly.  
static_assert(sizeof(CPR_Packet) <= 1472, "CPR_Packet too large! Will cause fragmentation.");