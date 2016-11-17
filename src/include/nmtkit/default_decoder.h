#ifndef NMTKIT_DEFAULT_DECODER_H_
#define NMTKIT_DEFAULT_DECODER_H_

#include <boost/serialization/base_object.hpp>
#include <dynet/lstm.h>
#include <dynet/model.h>
#include <nmtkit/decoder.h>
#include <nmtkit/encoder.h>
#include <nmtkit/multilayer_perceptron.h>
#include <nmtkit/serialization_utils.h>

namespace nmtkit {

class DefaultDecoder : public Decoder {
  DefaultDecoder(const DefaultDecoder &) = delete;
  DefaultDecoder(DefaultDecoder &&) = delete;
  DefaultDecoder & operator=(const DefaultDecoder &) = delete;
  DefaultDecoder & operator=(DefaultDecoder &&) = delete;

public:
  // Initializes an empty decoder.
  DefaultDecoder() {}

  // Initializes a decoder.
  //
  // Arguments:
  //   vocab_size: Vocabulary size of the input sequences.
  //   embed_size: Number of units in the embedding layer.
  //   hidden_size: Number of units in each RNN hidden layer.
  //   seed_size: Number of units in the seed vector.
  //   context_size: Number of units in the context vector.
  //   model: Model object for training.
  DefaultDecoder(
      unsigned vocab_size,
      unsigned embed_size,
      unsigned hidden_size,
      unsigned seed_size,
      unsigned context_size,
      dynet::Model * model);

  ~DefaultDecoder() override {}

  Decoder::State prepare(
      const dynet::expr::Expression & seed,
      dynet::ComputationGraph * cg) override;

  Decoder::State oneStep(
      const Decoder::State & state,
      const std::vector<unsigned> & input_ids,
      Attention * attention,
      dynet::ComputationGraph * cg,
      dynet::expr::Expression * atten_probs,
      dynet::expr::Expression * output) override;

  unsigned getOutputSize() const override { return hidden_size_; }

private:
  // Boost serialization interface.
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, const unsigned) {
    ar & boost::serialization::base_object<Decoder>(*this);
    ar & vocab_size_;
    ar & embed_size_;
    ar & hidden_size_;
    ar & seed_size_;
    ar & context_size_;
    ar & enc2dec_;
    ar & rnn_;
    ar & p_lookup_;
  }

  unsigned vocab_size_;
  unsigned embed_size_;
  unsigned hidden_size_;
  unsigned seed_size_;
  unsigned context_size_;
  MultilayerPerceptron enc2dec_;
  dynet::LSTMBuilder rnn_;
  dynet::LookupParameter p_lookup_;
};

}  // namespace nmtkit

NMTKIT_SERIALIZATION_DECL(nmtkit::DefaultDecoder);

#endif  // NMTKIT_DEFAULT_DECODER_H_
