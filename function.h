#ifndef FUNCTIONAL_FUNCTIONAL_H
#define FUNCTIONAL_FUNCTIONAL_H

#include <cstddef>
#include <memory>
#include <utility>

template<typename>
class function;

template<typename retT, typename ... argT>
class function<retT(argT ...)> {
private:
    class father {
    public:
        father() = default;

        virtual ~father() = default;

        virtual retT call(argT ... args) = 0;
    };

    typedef std::unique_ptr<father> ref_f;

//_______________________________________________________________
    template<typename F>
    class son : public father {
    public:
        son(F& f) : father(), _fun(f) {}

        retT call(argT ... args) override {
            return _fun(std::forward<argT>(args) ...);
//            return _fun(args ...);
        }

    private:
        F _fun;
    };
//________________________________________________________________
private:
    static const size_t BUF_SIZE = 10000;

    union {
        ref_f _call;
        unsigned char buffer[BUF_SIZE];
    } ;

    bool is_small;
//________________________________________________________________
public:
    function() noexcept {
        is_small = false;
        new(&_call) ref_f(nullptr);
    }

    function(std::nullptr_t) noexcept { 
        function();
    }

    function(const function &other) : is_small(other.is_small) {
        if (is_small) {
            std::copy(other.buffer,
                      other.buffer + BUF_SIZE, buffer);
        } else {
            new(&_call) ref_f(other._call.get());
        }
    }

    function(function &&other) noexcept : is_small(other.is_small) {

        if (is_small) {
            std::copy(other.buffer,
                      other.buffer + BUF_SIZE, buffer);
        } else {
            new(&_call) ref_f(other._call.get());
        }

//        if(is_small) {
//            if(other.is_small) {
//                std::copy(other.buffer,
//                          other.buffer + BUF_SIZE, buffer);
//                //(reinterpret_cast<father *>(other.buffer))->~father();//
//            } else {
//                new(&_call) ref_f(std::move(other._call));
//                is_small = false;
//            }
//        } else {
//            if(other.is_small) {
//                std::copy(other.buffer,
//                          other.buffer + BUF_SIZE, buffer);
//                //(reinterpret_cast<father *>(other.buffer))->~father();//
//                is_small = true;
//            } else {
//                new(&_call) ref_f(std::move(other._call));
//                is_small = false;
//            }
//        }




        //
//        if (is_small) {
//            std::copy(other.buffer,
//                      other.buffer + BUF_SIZE, buffer);
//        } else {
//            new(&_call) ref_f(std::move(other._call));
//            new(&other._call) ref_f(nullptr);
//        }

    }

    template<typename F>
    function(F f) { 
        if (sizeof(f) <= BUF_SIZE) {
            is_small = true;
            new(&buffer) son<F>(f);
        } else {
            is_small = false;
            new(&_call) std::unique_ptr<son<F>>(new son<F>(f));
        }
    }

    ~function() {
        if (is_small) {
            (reinterpret_cast<father *>(buffer))->~father();
        }
    }

    function &operator=(const function &other) {
        function tmp(other);
        swap(tmp);
        return *this;
    }

    function &operator=(function &&other) noexcept {
//        if (!is_small) {
//            _call.reset();
//        }
//        swap(other);
//
//
//        return *this;


//        if (is_small) {
//            auto c = reinterpret_cast<concept *>(buf);
//            c->~concept();
//        } else {
//            pconcept.~unique_ptr();
//        }
//        if (other.is_small) {
//            auto c = reinterpret_cast<concept *>(other.buf);
//            c->placement_build_small_move(buf);
//            new(other.buf) std::unique_ptr<concept>(nullptr);
//        } else {
//            new(buf) std::unique_ptr<concept>(std::move(other.pconcept));
//        }
//        is_small = other.is_small;
//        other.is_small = false;



//if(other.is_small) {
//    std::copy(other.buffer,
//              other.buffer + BUF_SIZE, buffer);
//    is_small = true;
//} else {
//    new(&_call) ref_f(std::move(other._call));
//    is_small = false;
//}

        function tmp(other);
        swap(tmp);
        return *this;

    }

    void swap(function &rhs) noexcept {
        if (is_small && rhs.is_small) {
            for (size_t i = 0; i < BUF_SIZE; i++) {
                std::swap(buffer[i], rhs.buffer[i]);
            }
        } else if (!is_small && !rhs.is_small) {
            _call.swap(rhs._call);
        } else if (is_small) {
            //swap_small_big(, rhs.);
            unsigned char tmp[BUF_SIZE];
            std::copy(buffer, buffer + BUF_SIZE, tmp);
            new(&_call) ref_f(rhs._call.get());
            rhs._call.reset();
            std::copy(tmp, tmp + BUF_SIZE, rhs.buffer);
        } else {
            //swap_small_big(rhs., );
            unsigned char tmp[BUF_SIZE];
            std::copy(rhs.buffer, rhs.buffer + BUF_SIZE, tmp);
            new(&rhs._call) ref_f(_call.get());
            _call.reset();
            std::copy(tmp, tmp + BUF_SIZE, buffer);
        }
        std::swap(is_small, rhs.is_small);
    }

    explicit operator bool() const noexcept {
        return is_small || _call;
    }

    retT operator()(argT ... args) const {
        if (is_small) {
            //return (reinterpret_cast<father *>(const_cast<unsigned char *>(buffer)))->call(
            return ((father *)(const_cast<unsigned char *>(buffer)))->call(
                    std::forward<argT>(args)...);
        } else {
            return _call->call(std::forward<argT>(args) ...);
        }
    }
//_______________________________________________________________
private:
//    void swap_small_big(any_data &small, any_data &big) {
//        unsigned char tmp[BUF_SIZE];
//        std::copy(small.buffer, small.buffer + BUF_SIZE, tmp);
//        new(&small._call) ref_f(big._call.get());
//        big._call.reset();
//        std::copy(tmp, tmp + BUF_SIZE, big.buffer);
//    }
};

#endif //FUNCTIONAL_FUNCTIONAL_H