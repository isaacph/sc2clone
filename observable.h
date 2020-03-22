//
// Created by isaac on 12/18/2019.
//

#ifndef UNTITLED2_OBSERVABLE_H
#define UNTITLED2_OBSERVABLE_H
#include <functional>
#include <map>
#include <iostream>
#include <set>

struct unit {};

class Subscription;

template <typename T>
class Observable {
public:
    virtual Subscription subscribe(std::function<void(T)> subscriber) = 0;
};

class UnsubscribeDestructor {
public:
    virtual ~UnsubscribeDestructor() {}
};

class Subscription {
    std::set<std::shared_ptr<UnsubscribeDestructor>> unsubFunc;
public:
    explicit inline Subscription() = default;
    inline Subscription(UnsubscribeDestructor* f) : unsubFunc({std::shared_ptr<UnsubscribeDestructor>(f)}) {}
    Subscription& operator+=(const Subscription& other) {
        unsubFunc.insert(other.unsubFunc.begin(), other.unsubFunc.end());
        return *this;
    }
    Subscription operator+(const Subscription& other) {
        Subscription s(*this);
        s += other;
        return s;
    }
};

template<typename T>
class Subscribable : public Observable<T> {
public:
    Subscription subscribe(std::function<void(T)> subscriber) override {
        int index = subIndex++;
        subscribers[index] = std::move(subscriber);
        return Subscription(new UnsubscribeDestructor(*this, index));
    }
private:
    class UnsubscribeDestructor : public ::UnsubscribeDestructor {
        int unsubIndex;
        Subscribable<T>& emitter;
    public:
        explicit UnsubscribeDestructor(Subscribable<T>& emitter_in, int unsubIndex_in) :
        emitter(emitter_in), unsubIndex(unsubIndex_in) {}
        virtual inline ~UnsubscribeDestructor() override {
            emitter.subscribers.erase(unsubIndex);
        }
    };
    friend UnsubscribeDestructor;
    int subIndex = 0;
protected:
    std::map<int, std::function<void(T)>> subscribers;
};

template <typename T>
class EventEmitter : public Subscribable<T> {
public:
    virtual void emit(const T& t) {
        for(const std::pair<int, std::function<void(T)>>& pair : this->subscribers) {
            pair.second(t);
        }
    }
};

template <typename T>
class Observer : public Subscribable<T> {
    Subscription observeSub;
public:
    Observer(Observable<T>& observable) :
        observeSub(observable.subscribe([&](T t) {
            for(const std::pair<int, std::function<void(T)>>& pair : this->subscribers) {
                pair.second(t);
            }
        })) {}
};

#endif //UNTITLED2_OBSERVABLE_H
