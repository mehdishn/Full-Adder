#include "systemc.h"

class XORgate : public sc_module
{
public:
    sc_port<sc_signal_in_if<sc_logic>, 1> i1, i2;
    sc_port<sc_signal_out_if<sc_logic>, 1> o1;

    SC_CTOR(XORgate)
    {
        SC_METHOD(eval);
        sensitive << i1 << i2;
    }
    void eval()
    {
        if (i1->read() == i2->read())
            o1->write(SC_LOGIC_0);
        else
            o1->write(SC_LOGIC_1);
    }
};

class AO2gate : public sc_module
{
public:
    sc_port<sc_signal_in_if<sc_logic>, 1> i1, i2, i3, i4;
    sc_port<sc_signal_out_if<sc_logic>, 1> o1;

    SC_CTOR(AO2gate)
    {
        SC_METHOD(eval);
        sensitive << i1 << i2 << i3 << i4;
    }

    void eval()
    {
        if ((i1->read() & i2->read()) == '1' | (i3->read() & i4->read()) == '1')
            o1->write(SC_LOGIC_1);
        else
            o1->write(SC_LOGIC_0);
    }
};

class oneBitAdder : public sc_module
{
public:
    sc_port<sc_signal_in_if<sc_logic>, 1> i1, i2, i3;
    sc_port<sc_signal_out_if<sc_logic>, 1> o1, o2;

    sc_signal<sc_logic> x1;

    XORgate* XORa;
    XORgate* XORb;
    AO2gate* AO2a;

    SC_HAS_PROCESS(oneBitAdder);
    oneBitAdder(sc_module_name)
    {
        XORa = new XORgate("xor_insta");
        (*XORa) (i1, i2, x1);
        XORb = new XORgate("xor_instb");
        (*XORb) (x1, i3, o2);
        AO2a = new AO2gate("ao2_insta");
        (*AO2a) (i1, i2, x1, i3, o1);
    }
};

int sc_main(int, char*[])
{
    sc_signal<sc_logic> a;
    sc_signal<sc_logic> b;
    sc_signal<sc_logic> ci;
    sc_signal<sc_logic> s;
    sc_signal<sc_logic> co;

    oneBitAdder* fullAdder = new oneBitAdder("FA");
    (*fullAdder) (a, b, ci, co, s);

    sc_trace_file* VCDFile;
    VCDFile = sc_create_vcd_trace_file("oneBitAdder_waveform");
    sc_trace(VCDFile, a, "A_in");
    sc_trace(VCDFile, b, "B_in");
    sc_trace(VCDFile, ci, "Carry_in");
    sc_trace(VCDFile, co, "Carry_out");
    sc_trace(VCDFile, s, "Sum_out");

    sc_start(50, SC_NS);

    sc_int<3> int_input_data = 0;
    sc_lv<3> logic_input_data;

    for (int i = 0; i < 40; i++)
    {
        logic_input_data = int_input_data;
        a = logic_input_data[0];
        b = logic_input_data[1];
        ci = logic_input_data[2];

        int_input_data++;
        sc_start(200, SC_NS);
    }

    return 0;

}
