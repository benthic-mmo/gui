#ifndef LXGUI_GUI_EVENTMANAGER_HPP
#define LXGUI_GUI_EVENTMANAGER_HPP

#include <lxgui/utils.hpp>
#include <string>
#include <vector>
#include <list>

namespace lxgui {
namespace gui
{
    class event_receiver;
    class event;

    /// Manages events and their responses
    class event_manager
    {
    public :

        /// Enables an event_receiver's reaction to an event.
        /** \param pReceiver The event_receiver to consider
        *   \param sEvent    The name of the event it should react to
        */
        void register_event(event_receiver* pReceiver, const std::string& sEvent);

        /// Disables an event_receiver's reaction to an event.
        /** \param pReceiver The event_receiver to consider
        *   \param sEvent    The name of the event it shouldn't react to anymore
        */
        void unregister_event(event_receiver* pReceiver, const std::string& sEvent);

        /// Disables all events connected to the provided event_receiver.
        /** \param pReceiver The event_receiver to disable
        */
        void unregister_receiver(event_receiver* pReceiver);

        /// Tells this manager an Event as occured.
        /** \param mEvent The Event which has occured
        *   \note All event_receivers registred to react to this Event
        *         will be told the Event has occured by calling
        *         event_receiver::on_event().
        */
        void fire_event(const event& mEvent);

        /// Tells this manager to clear the fired Events list.
        void frame_ended();

    private :

        struct registered_event
        {
            std::string                  sName;
            bool                         bFired = false;
            std::list<event_receiver*>   lReceiverList;
        };

        std::list<registered_event> lRegisteredEventList_;
    };
}
}

#endif
