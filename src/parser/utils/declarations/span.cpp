class Span {
public:
    explicit Span(int start, int end): start(start), end(end) {};
    explicit Span(int start): start(start), end(-1) {};
    int start, end;
};