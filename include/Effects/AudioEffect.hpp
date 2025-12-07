#ifndef AUDIO_EFFECT_HPP_
#define AUDIO_EFFECT_HPP_

#include "project.h"
#include "AudioBuffer.hpp"

namespace audio
{
    namespace effects
    {

        /**
         * Base class for all audio effects
         * Uses CRTP pattern for zero-cost abstraction
         */
        template <typename SampleType>
        class AudioEffect
        {
        public:
            virtual ~AudioEffect() = default;

            /**
             * Process audio buffer in-place
             * @param buffer Audio buffer to process
             */
            virtual void process(AudioBuffer<SampleType> &buffer) = 0;

            /**
             * Reset internal state (clear history, buffers, etc.)
             */
            virtual void reset() = 0;

            /**
             * Get effect name (for debugging/display)
             */
            virtual const char *name() const { return "AudioEffect"; }

            /**
             * Check if effect is enabled
             */
            bool is_enabled() const { return enabled_; }

            /**
             * Enable/disable effect
             */
            void set_enabled(bool enabled) { enabled_ = enabled; }

        protected:
            bool enabled_ = true;
        };

    } // namespace effects
} // namespace audio

#endif // AUDIO_EFFECT_HPP_
